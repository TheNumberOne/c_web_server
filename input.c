#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include <zconf.h>
#include "input.h"


result_t readUntilChars(int fd, string_t *pString, const char *chars) {
    size_t charsLength = strlen(chars);
    assert(charsLength > 0);

    result_t ret;

    // Basic state machine
    bool *matchedChar = malloc(charsLength * sizeof(bool));

    if (matchedChar == NULL) return OOM_ERROR;

    for (size_t i = 0; i < charsLength; i++) matchedChar[i] = false;


    *pString = createString();
    if (*pString == NULL) {
        ret = OOM_ERROR;
        goto freeMatched;
    }

    while (true) {
        char c;
        ssize_t readRet = read(fd, &c, 1);
        if (readRet == 0) {
            ret = EOF_ERROR;
            goto freeString;
        }

        if (readRet < 0) {
            ret = IO_ERROR;
            goto freeString;
        }

        if (matchedChar[charsLength - 2] && chars[charsLength - 1] == c) {
            removeLastChars(*pString, charsLength - 1);
            ret = OK;
            goto freeMatched;
        }

        if (append(*pString, c) < 0) {
            ret = OOM_ERROR;
            goto freeString;
        }


        for (size_t i = charsLength - 1; i > 0; i--) {
            matchedChar[i] = (matchedChar[i - 1] && c == chars[i]);
        }
        matchedChar[0] = c == chars[0];
    }

    freeString:
    destroyString(*pString);
    freeMatched:
    free(matchedChar);
    return ret;
}

result_t readUntil(int fd, string_t *pString, char c) {
    *pString = createString();

    size_t i = 0;
    while (true) {
        // Out of memory error.
        if (*pString == NULL) return OOM_ERROR;

        //Read next character.
        char next;
        ssize_t bytesRead = read(fd, &next, 1);

        // Check if EOF
        if (bytesRead == 0) {
            destroyString(*pString);
            return EOF_ERROR;
        }

        // Error reading file.
        if (bytesRead < 0) {
            destroyString(*pString);
            return IO_ERROR;
        }

        // Stop reading if next character is the one looked for
        if (next == c) {
            return OK;
        }

        // Append the character
        if (append(*pString, next) < 0) {
            destroyString(*pString);
            return OOM_ERROR;
        }

        i++;
    }
}