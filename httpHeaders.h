#pragma once

#include <stddef.h>
#include "string.h"

struct httpHeaders;
struct httpHeader;

typedef struct httpHeader *httpHeader_t;
typedef struct httpHeaders *httpHeaders_t;

httpHeaders_t createHttpHeaders();

/**
 * Appends a header with the specified key and value to this headers object.
 * Takes ownership of both key and value.
 */
void httpHeadersAppend(httpHeaders_t self, string_t key, string_t value);

void destroyHttpHeaders(httpHeaders_t self);

/**
 * Returns the key (and not a copy of it) of this header item.
 * Ownership retained by the header.
 */
string_t httpHeaderKey(httpHeader_t);

/**
 * Returns the value (and not a copy of it) of this header item.
 * Ownership retained by the header.
 */
string_t httpHeaderValue(httpHeader_t);

/**
 * Returns the next header or NULL if there is no next header.
 * Ownership of returned object is not given to caller.
 */
httpHeader_t httpHeaderNext(httpHeader_t);

/**
 * Returns the first header in this headers object.
 * Ownership of returned object retained.
 */
httpHeader_t httpHeadersFirst(httpHeaders_t);