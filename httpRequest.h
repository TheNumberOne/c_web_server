#pragma once

#include <malloc.h>
#include "string.h"
#include "uri.h"
#include "httpHeaders.h"

/**
 * Represents an http request. Content is not implemented currently.
 */
struct httpRequest {
    /**
     * The method that is being called.
     * Should probably be an enum.
     */
    string_t method;

    /**
     * The URI specified as the target.
     */
    uri_t target;

    /**
     * The http version
     */
    string_t httpVersion;

    /**
     * The headers in the request.
     */
    httpHeaders_t headers;
};


typedef struct httpRequest *httpRequest_t;

void destroyHttpRequest(httpRequest_t self);

/**
 * Creates an http request with the specified parameters.
 * Takes ownership of all parameters passed.
 */
httpRequest_t createHttpRequest(
        string_t method,
        uri_t target,
        string_t httpVersion,
        httpHeaders_t headers
);

/**
 * Reads a request from the specified file descriptor.
 *
 * @param fd The file descriptor that the request is read from. Not closed.
 * @param request The request is returned at the location pointed at.
 * @return NULL if reading the request went fine. A response if there was some
 * sort of error appropriate for the error.
 */
httpResponse_t readRequest(int fd, httpRequest_t *request);