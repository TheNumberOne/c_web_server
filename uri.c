#include <malloc.h>
#include <assert.h>
#include <stdbool.h>
#include "uri.h"

int hexDigitToInt(char digit);

uri_t parseUri(string_t s) {
    if (stringLength(s) == 0 || *charAt(s, 0) != '/') return NULL;

    size_t numParts = 0;
    for (size_t i = 0; i < stringLength(s); i++) {
        if (*charAt(s, i) == '/') {
            numParts++;
        }
    }
    uri_t result = malloc(sizeof(struct uri));
    result->parts = malloc(sizeof(string_t) * numParts);
    result->numParts = numParts;

    size_t j = 0;
    for (size_t i = 0; i < numParts; i++) {
        result->parts[i] = createString();
        j++;
        while (j < stringLength(s) && *charAt(s, j) != '/') {
            if (*charAt(s, j) == '%') {
                char c = hexDigitToInt(*charAt(s, j + 1)) * 16 + hexDigitToInt(*charAt(s, j + 2));
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

void destroyUri(uri_t uri) {
    for (size_t i = 0; i < uri->numParts; i++) {
        destroyString(uri->parts[i]);
    }
    free(uri->parts);
    free(uri);
}