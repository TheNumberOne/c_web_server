#pragma once

#include "string.h"

/**
 * Represents a uri
 */
struct uri {
    string_t *parts;
    size_t numParts;
};

typedef struct uri *uri_t;

/**
 * Parses an absolute uri. e.g. /atat/th/ath
 * @param s does not take ownership
 */
uri_t parseUri(string_t s);

void destroyUri(uri_t i);

/**
 * Converts the uri to a string.
 */
string_t uriToString(uri_t uri);

/**
 * Converts the uri to a relative file path.
 * all leading slashes are stripped. returns null if it
 * can't be made into a valid filepath or anything along the path
 * starts with . (no hidden files)
 * @param path not taken ownership
 * @return ownership passed to caller
 */
char *uriToFilePath(uri_t path);