#include "http.h"
#include "string.h"
#include "response.h"
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include <assert.h>

httpResponse_t readRequest(int fd);

enum result {
    OK,
    IO_ERROR,
    OOM_ERROR,
    EOF_ERROR,
    INVALID_METHOD,
    INVALID_VERSION
};
typedef enum result result_t;

result_t readMethod(int fd, string_t *methodPtr);

result_t readTarget(int fd, string_t *pTarget);

result_t readUntil(int fd, string_t *pString, char c);

result_t readHttpVersion(int fd, string_t *pString);

result_t readUntilChars(int fd, string_t *pString, const char *chars);

void writeResponse(int fd, httpResponse_t response);

void writeString(int fd, string_t str);

void handleHttpConnection(int socketFd) {
    httpResponse_t response = readRequest(socketFd);
    if (response != NULL) {
        writeResponse(socketFd, response);
        close(socketFd);
        destroyHttpResponse(response);
        return;
    }
    close(socketFd);
}

void writeResponse(int fd, httpResponse_t response) {
    writeString(fd, response->httpVersion);
    write(fd, " ", 1);
    dprintf(fd, "%03u", response->statusCode);
    write(fd, " ", 1);
    writeString(fd, response->reasonPhrase);
    write(fd, "\r\n", 2);
    for (size_t i = 0; i < response->header.numHeaders; i++) {
        writeString(fd, response->header.headers[i].key);
        write(fd, ": ", 2);
        writeString(fd, response->header.headers[i].val);
        write(fd, "\r\n", 2);
    }
    write(fd, "\r\n", 2);
    writeString(fd, response->content);
}

void writeString(int fd, string_t str) {
    write(fd, stringData(str), stringLength(str));
}

httpResponse_t readRequest(int fd) {
    string_t method;
    result_t r = readMethod(fd, &method);

    if (r == INVALID_METHOD) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_INVALID_METHOD);
        return response;
    }

    string_t target;
    readTarget(fd, &target);


    string_t version;
    r = readHttpVersion(fd, &version);

    if (r == INVALID_VERSION) {
        httpResponse_t response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_INVALID_VERSION);
        return response;
    }
    printf("Method: %s\nTarget: %s\nVersion: %s\n", charAt(method, 0), charAt(target, 0), charAt(version, 0));
    return NULL;
}

result_t readHttpVersion(int fd, string_t *pString) {
    readUntilChars(fd, pString, "\r\n");
    if (strcmp("HTTP/1.1", stringData(*pString)) != 0) {
        destroyString(*pString);
        return INVALID_VERSION;
    }
    return OK;
}

result_t readUntilChars(int fd, string_t *pString, const char *chars) {
    size_t charsLength = strlen(chars);
    assert(charsLength > 0);

    result_t ret;

    // Basic state machine
    bool *matchedChar = malloc(charsLength * sizeof(bool));

    if (matchedChar == NULL) return OOM_ERROR;

    for (size_t i = 0; i < charsLength; i++) matchedChar[i] = false;


    *pString = createString();
    if (*pString == NULL) {
        ret = OOM_ERROR;
        goto freeMatched;
    }

    while (true) {
        char c;
        ssize_t readRet = read(fd, &c, 1);
        if (readRet == 0) {
            ret = EOF_ERROR;
            goto freeString;
        }

        if (readRet < 0) {
            ret = IO_ERROR;
            goto freeString;
        }

        if (matchedChar[charsLength - 2] && chars[charsLength - 1] == c) {
            removeLastChars(*pString, charsLength - 1);
            ret = OK;
            goto freeMatched;
        }

        if (append(*pString, c) < 0) {
            ret = OOM_ERROR;
            goto freeString;
        }


        for (size_t i = charsLength - 1; i > 0; i--) {
            matchedChar[i] = (matchedChar[i - 1] && c == chars[i]);
        }
        matchedChar[0] = c == chars[0];
    }

    freeString:
    destroyString(*pString);
    freeMatched:
    free(matchedChar);
    return ret;
}

result_t readTarget(int fd, string_t *pTarget) {
    return readUntil(fd, pTarget, ' ');
}

result_t readUntil(int fd, string_t *pString, char c) {
    *pString = createString();

    size_t i = 0;
    while (true) {
        // Out of memory error.
        if (*pString == NULL) return OOM_ERROR;

        //Read next character.
        char next;
        ssize_t bytesRead = read(fd, &next, 1);

        // Check if EOF
        if (bytesRead == 0) {
            destroyString(*pString);
            return EOF_ERROR;
        }

        // Error reading file.
        if (bytesRead < 0) {
            destroyString(*pString);
            return IO_ERROR;
        }

        // Stop reading if next character is the one looked for
        if (next == c) {
            return OK;
        }

        // Append the character
        if (append(*pString, next) < 0) {
            destroyString(*pString);
            return OOM_ERROR;
        }

        i++;
    }
}


result_t readMethod(int fd, string_t *methodPtr) {
    readUntil(fd, methodPtr, ' ');
    if (strcmp(charAt(*methodPtr, 0), "GET") != 0) {
        destroyString(*methodPtr);
        return INVALID_METHOD;
    }
    return OK;
}
