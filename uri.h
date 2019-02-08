#ifndef PROJECT2_URI_H
#define PROJECT2_URI_H

#include "string.h"

struct uri {
    string_t *parts;
    size_t numParts;
};


typedef struct uri *uri_t;

uri_t parseUri(string_t);

void destroyUri(uri_t i);

string_t uriToString(uri_t uri);
#endif //PROJECT2_URI_H
