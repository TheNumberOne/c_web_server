#pragma once

#include <stddef.h>
#include "string.h"

struct httpHeaders;
struct httpHeader;

typedef struct httpHeader *httpHeader_t;
typedef struct httpHeaders *httpHeaders_t;

httpHeaders_t createHttpHeaders();

void httpHeadersAppend(httpHeaders_t self, string_t key, string_t value);

void destroyHttpHeaders(httpHeaders_t self);

string_t httpHeaderKey(httpHeader_t);
string_t httpHeaderValue(httpHeader_t);
httpHeader_t httpHeaderNext(httpHeader_t);
httpHeader_t httpHeadersFirst(httpHeaders_t);