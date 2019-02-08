//
// Created by rose on 2/7/19.
//

#include "httpRequest.h"


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
