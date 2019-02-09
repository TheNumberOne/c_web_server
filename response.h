#pragma once

#include <glob.h>
#include "string.h"
#include "httpHeaders.h"

/**
 * Different possible status codes for an http response.
 */
enum httpStatusCode {
    HTTP_STATUS_CODE_UNINITIALIZED = 0,
    HTTP_STATUS_CODE_INVALID_METHOD = 405,
    HTTP_STATUS_CODE_OK = 200,
    HTTP_STATUS_CODE_INVALID_VERSION = 505,
    HTTP_STATUS_CODE_INVALID_REQUEST = 400,
    HTTP_STATUS_CODE_NOT_FOUND = 401,
    HTTP_STATUS_CODE_SERVER_ERROR = 500
};
typedef enum httpStatusCode httpStatusCode_t;

struct httpResponse {
    string_t httpVersion;
    httpStatusCode_t statusCode;
    string_t reasonPhrase;
    httpHeaders_t header;
    string_t content;
};


typedef struct httpResponse *httpResponse_t;

httpResponse_t createHttpResponse();

void destroyHttpResponse(httpResponse_t t);

/**
 * Sets the status of the response to the specified code.
 * Sets the reason phrase as well.
 */
void httpResponseStatus(httpResponse_t this, httpStatusCode_t status);

/**
 * Adds the content length header to this response. Will not remove it if it has
 * previously been added.
 */
void addContentLengthHeader(httpResponse_t this);

/**
 * Adds a header to the response
 * @param key taken ownership by \p response
 * @param val taken ownership by \p response
 */
void addHeader(httpResponse_t response, string_t key, string_t val);

/**
 * Sets the content to the specified value.
 * @param content taken ownership by \p self
 */
void setHttpContent(httpResponse_t self, string_t content);

/**
 * Writes the response out.
 * @param fd The file to write it out to
 * @param response The response.
 */
void writeResponse(int fd, httpResponse_t response);