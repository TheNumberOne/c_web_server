#include <malloc.h>
#include "httpHeaders.h"

struct httpHeader {
    string_t key;
    string_t val;
    struct httpHeader *next;
};

struct httpHeaders {
    httpHeader_t headers;
    size_t numHeaders;
};

httpHeaders_t createHttpHeaders() {
    httpHeaders_t ret = malloc(sizeof(struct httpHeader));
    ret->headers = NULL;
    ret->numHeaders = 0;
    return ret;
}

void httpHeadersAppend(httpHeaders_t self, string_t key, string_t value) {
    httpHeader_t header = malloc(sizeof(struct httpHeader));
    header->key = key;
    header->val = value;
    header->next = self->headers;
    self->headers = header;
    self->numHeaders++;
}

void destroyHttpHeaders(httpHeaders_t self) {
    httpHeader_t header = self->headers;

    while (header != NULL) {
        httpHeader_t next = header->next;
        destroyString(header->key);
        destroyString(header->val);
        free(header);
        header = next;
    }

    free(self);
}

string_t httpHeaderKey(httpHeader_t self) {
    return self->key;
}

string_t httpHeaderValue(httpHeader_t self) {
    return self->val;
}

httpHeader_t httpHeaderNext(httpHeader_t self) {
    return self->next;
}

httpHeader_t httpHeadersFirst(httpHeaders_t self) {
    return self->headers;
}
