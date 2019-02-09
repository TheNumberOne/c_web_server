#include <malloc.h>
#include <assert.h>
#include <stdbool.h>
#include <memory.h>
#include "uri.h"

int hexDigitToInt(char digit);

uri_t parseUri(string_t s) {
    // Make sure uri starts with /
    if (stringLength(s) == 0 || *charAt(s, 0) != '/') return NULL;

    // count the number of parts.
    size_t numParts = 0;
    for (size_t i = 0; i < stringLength(s); i++) {
        if (*charAt(s, i) == '/') {
            numParts++;
        }
    }

    // allocate the parts
    uri_t result = malloc(sizeof(struct uri));
    result->parts = malloc(sizeof(string_t) * numParts);
    result->numParts = numParts;

    // copy the parts in
    size_t j = 0;
    for (size_t i = 0; i < numParts; i++) {
        result->parts[i] = createString();
        j++;
        // read until the next /
        while (j < stringLength(s) && *charAt(s, j) != '/') {
            // converts % into their other form. \0 and / are allowed.
            if (*charAt(s, j) == '%') {
                // make sure % isn't too close to end of string
                if (j + 2 >= stringLength(s)) {
                    for (; i >= 0; i--) {
                        destroyString(result->parts[i]);
                    }
                    free(result->parts);
                    free(result);
                    return NULL;
                }
                // parse hex digits
                char c = (char) (hexDigitToInt(*charAt(s, j + 1)) * 16 + hexDigitToInt(*charAt(s, j + 2)));
                append(result->parts[i], c);
                j += 3;
                continue;
            }
            append(result->parts[i], *charAt(s, j));
            j++;
        }
    }
    return result;
}


int hexDigitToInt(char digit) {
    assert('0' <= digit && digit <= '9' || 'A' <= digit && digit <= 'F' || 'a' <= digit && digit <= 'f');

    if ('0' <= digit && digit <= '9') {
        return digit - '0';
    }
    if ('A' <= digit && digit <= 'F') {
        return digit - 'A' + 10;
    }
    if ('a' <= digit && digit <= 'f') {
        return digit - 'a' + 10;
    }

    // Shouldn't have gotten here
    assert(false);
}

string_t uriToString(uri_t uri) {
    string_t s = createString();
    for (size_t i = 0; i < uri->numParts; i++) {
        if (i != 0) append(s, '/');
        plusEqual(s, uri->parts[i]);
    }
    return s;
}

void destroyUri(uri_t uri) {
    for (size_t i = 0; i < uri->numParts; i++) {
        destroyString(uri->parts[i]);
    }
    free(uri->parts);
    free(uri);
}

char *uriToFilePath(uri_t path) {
    //Initial size 1 for null char
    size_t pathSize = 1;
    bool first = true;
    for (size_t i = 0; i < path->numParts; i++) {
        string_t s = path->parts[i];
        if (stringLength(s) == 0) continue;

        // Don't reveal hidden files.
        if (*charAt(s, 0) == '.') {
            return NULL;
        }

        // Check for invalid characters (/ and NULL) in the path.
        for (size_t j = 0; j < stringLength(s); j++) {
            char c = *charAt(s, j);
            if (c == '/' || c == 0) {
                return NULL;
            }
        }

        pathSize += stringLength(s);

        // Every element but first has slash in front.
        if (!first) {
            pathSize++;
        } else if (stringLength(s) > 0) {
            // Skip past leading slashes
            first = false;
        }
    }

    // build path
    char *p = malloc(pathSize * sizeof(char));
    p[pathSize - 1] = '\0';
    char *j = p;
    first = true; // first is set to false once past leading slashes
    for (size_t i = 0; i < path->numParts; i++) {
        if (!first) {
            *(j++) = '/'; // only add slashes if they aren't leading
        } else if (stringLength(path->parts[i]) > 0) {
            first = false;
        }

        memcpy(j, stringData(path->parts[i]), stringLength(path->parts[i]));
        j += stringLength(path->parts[i]);
    }

    return p;
}