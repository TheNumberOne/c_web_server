#ifndef PROJECT2_RESPONSE_H
#define PROJECT2_RESPONSE_H


#include <glob.h>
#include "string.h"

struct httpHeader {
    string_t key;
    string_t val;
};

typedef struct httpHeader httpHeader_t;
struct httpHeaders {
    httpHeader_t *headers;
    size_t numHeaders;
};
typedef struct httpHeaders httpHeaders_t;

enum httpStatusCode {
    HTTP_STATUS_CODE_INVALID_METHOD = 405,
    HTTP_STATUS_CODE_OK = 200,
    HTTP_STATUS_CODE_INVALID_VERSION = 505
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

void addContentHeader(httpResponse_t this);

httpResponse_t* serverError;

void destroyHttpResponse(httpResponse_t t);


#endif //PROJECT2_RESPONSE_H
