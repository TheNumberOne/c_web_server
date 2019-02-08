#pragma once

#include <malloc.h>
#include "string.h"
#include "uri.h"
#include "httpHeaders.h"

struct httpRequest {
    string_t method;
    uri_t target;
    string_t httpVersion;
    httpHeaders_t headers;
};


typedef struct httpRequest *httpRequest_t;

void destroyHttpRequest(httpRequest_t self);

httpRequest_t createHttpRequest(
        string_t method,
        uri_t target,
        string_t httpVersion,
        httpHeaders_t headers
        );
