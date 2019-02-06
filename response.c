#include <malloc.h>
#include "response.h"


void addHeader(httpResponse_t response, string_t key, string_t val);

httpResponse_t createHttpResponse() {
    httpResponse_t r = malloc(sizeof(struct httpResponse));
    r->content = createString();
    r->header.headers = NULL;
    r->header.numHeaders = 0;
    r->reasonPhrase = createString();
    r->statusCode = 0;
    r->httpVersion = stringFromCString("HTTP/1.1");
    return r;
}

void httpResponseStatus(httpResponse_t this, httpStatusCode_t status) {
    this->statusCode = status;
    switch (status) {
        case HTTP_STATUS_CODE_INVALID_METHOD:
            stringSetTo(this->reasonPhrase, "Invalid method");
            break;
        case HTTP_STATUS_CODE_OK:
            stringSetTo(this->reasonPhrase, "Ok");
            break;
        case HTTP_STATUS_CODE_INVALID_VERSION:
            stringSetTo(this->reasonPhrase, "Invalid version");
            break;
    }
}

void addHeader(httpResponse_t response, string_t key, string_t val) {
    response->header.headers = realloc(
            response->header.headers,
            (response->header.numHeaders + 1) * sizeof(httpHeader_t)
    );
    httpHeader_t *header = &response->header.headers[response->header.numHeaders];
    header->key = key;
    header->val = val;
}

void addContentHeader(httpResponse_t this) {
    size_t length = stringLength(this->content);
    if (length > 0) {
        addHeader(this, stringFromCString("Content-Length"), stringFromInt((int) length));
    }
}

void destroyHttpResponse(httpResponse_t t) {
    destroyString(t->reasonPhrase);
    destroyString(t->httpVersion);
    destroyString(t->content);
    for (size_t i = 0; i < t->header.numHeaders; i++) {
        destroyString(t->header.headers[i].key);
        destroyString(t->header.headers[i].val);
    }
    free(t->header.headers);
    free(t);
}


