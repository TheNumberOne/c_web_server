#ifndef PROJECT2_RESPONSE_H
#define PROJECT2_RESPONSE_H


#include <glob.h>
#include "string.h"
#include "httpHeaders.h"

enum httpStatusCode {
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


typedef struct httpResponse* httpResponse_t;

httpResponse_t createHttpResponse();

void httpResponseStatus(httpResponse_t this, httpStatusCode_t status);

void addContentLengthHeader(httpResponse_t this);

httpResponse_t* serverError;

void destroyHttpResponse(httpResponse_t t);

void addHeader(httpResponse_t response, string_t key, string_t val);

void setHttpContent(httpResponse_t self, string_t content);

#endif //PROJECT2_RESPONSE_H
