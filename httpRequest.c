//
// Created by rose on 2/7/19.
//

#include <memory.h>
#include <stdbool.h>
#include "uri.h"
#include "response.h"
#include "httpRequest.h"
#include "result.h"
#include "input.h"


result_t readHeaders(int fd, httpHeaders_t *headers);

result_t readHttpVersion(int fd, string_t *pString);

result_t readTarget(int fd, uri_t *uri);

result_t readMethod(int fd, string_t *methodPtr);

void destroyHttpRequest(httpRequest_t self) {
    destroyString(self->method);
    destroyUri(self->target);
    destroyString(self->httpVersion);
    destroyHttpHeaders(self->headers);
    free(self);
}

httpRequest_t createHttpRequest(string_t method, uri_t target, string_t httpVersion, httpHeaders_t headers) {
    httpRequest_t ret = malloc(sizeof(struct httpRequest));
    ret->method = method;
    ret->target = target;
    ret->httpVersion = httpVersion;
    ret->headers = headers;
    return ret;
}

httpResponse_t readRequest(int fd, httpRequest_t *request) {
    // First read method
    string_t method;
    result_t r = readMethod(fd, &method);

    if (r != OK) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_INVALID_METHOD);
        return response;
    }

    // and then target
    uri_t target;
    r = readTarget(fd, &target);

    if (target == NULL || r != OK) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_INVALID_REQUEST);
        return response;
    }

    // and then version
    string_t version;
    r = readHttpVersion(fd, &version);

    if (r == INVALID_VERSION) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_INVALID_VERSION);
        return response;
    } else if (r != OK) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_SERVER_ERROR);
        return response;
    }

    // and then headers
    httpHeaders_t headers;
    r = readHeaders(fd, &headers);

    if (r == INVALID_HEADER) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_INVALID_REQUEST);
        return response;
    }

    *request = createHttpRequest(method, target, version, headers);
    return NULL;
}

result_t readHeaders(int fd, httpHeaders_t *headers) {
    *headers = createHttpHeaders();

    while (true) {
        // first read the line
        string_t line;
        readUntilChars(fd, &line, "\r\n");

        // if line length is zero, that means we've reached
        // the newline indicating the headers are over.
        if (stringLength(line) == 0) {
            destroyString(line);
            return OK;
        }

        // Read the key
        string_t key = createString();
        size_t i = 0;
        for (;; i++) {
            if (i == stringLength(line)) {
                destroyString(key);
                destroyString(line);
                destroyHttpHeaders(*headers);
                return INVALID_HEADER;
            }
            if (*charAt(line, i) == ':') {
                i++;
                break;
            }
            append(key, *charAt(line, i));
        }

        // Skip past spaces
        for (; i < stringLength(line) && *charAt(line, i) == ' '; i++);

        // read the value
        string_t value = createString();
        size_t spaces = 0; // keep track of trailing spaces.
        for (; i < stringLength(line); i++) {
            char c = *charAt(line, i);
            append(value, c);
            if (c == ' ') {
                spaces++;
            } else {
                spaces = 0;
            }
        }

        //Trim trailing spaces.
        removeLastChars(value, spaces);
        destroyString(line);

        httpHeadersAppend(*headers, key, value);
    }
}

result_t readHttpVersion(int fd, string_t *pString) {
    result_t r = readUntilChars(fd, pString, "\r\n");

    if (r != OK) return r;

    // Version must be HTTP/1.1
    if (strcmp("HTTP/1.1", stringData(*pString)) != 0) {
        destroyString(*pString);
        return INVALID_VERSION;
    }
    return OK;
}

result_t readTarget(int fd, uri_t *uri) {
    string_t s;
    result_t ret;
    if ((ret = readUntil(fd, &s, ' ')) != OK) return ret;
    *uri = parseUri(s);
    destroyString(s);
    return OK;
}

enum result readMethod(int fd, string_t *methodPtr) {
    enum result ret = readUntil(fd, methodPtr, ' ');
    if (ret != OK) return ret;

    // We only allow a GET method for now
    if (strcmp(charAt(*methodPtr, 0), "GET") != 0) {
        destroyString(*methodPtr);
        return INVALID_METHOD;
    }
    return OK;
}