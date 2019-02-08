#include <malloc.h>
#include "response.h"

httpResponse_t createHttpResponse() {
    httpResponse_t r = malloc(sizeof(struct httpResponse));
    r->content = createString();
    r->header = createHttpHeaders();
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
        case HTTP_STATUS_CODE_INVALID_REQUEST:
            stringSetTo(this->reasonPhrase, "Invalid request");
            break;
        case HTTP_STATUS_CODE_NOT_FOUND:
            stringSetTo(this->reasonPhrase, "Not found");
            break;
        case HTTP_STATUS_CODE_SERVER_ERROR:
            stringSetTo(this->reasonPhrase, "Internal server error");
            break;
    }
}

void addHeader(httpResponse_t response, string_t key, string_t val) {
    httpHeadersAppend(response->header, key, val);
}

void addContentLengthHeader(httpResponse_t this) {
    size_t length = stringLength(this->content);
    if (length > 0) {
        addHeader(this, stringFromCString("Content-Length"), stringFromInt((int) length));
    }
}

void destroyHttpResponse(httpResponse_t t) {
    destroyString(t->reasonPhrase);
    destroyString(t->httpVersion);
    destroyString(t->content);
    destroyHttpHeaders(t->header);
    free(t);
}

void setHttpContent(httpResponse_t self, string_t content) {
    moveString(self->content, content);
}
