#include <memory.h>
#include "uri.h"
#include "mimeTypes.h"


const int numFileTypes = 8;

/**
 * The different file types we know about.
 */
const char fileTypes[8][2][16] = {
        {"html", "text/html"},
        {"js",   "text/javascript"},
        {"css",  "text/css"},
        {"png",  "image/png"},
        {"jpeg", "image/jpeg"},
        {"jpg",  "image/jpeg"},
        {"pdf",  "application/pdf"},
        // Php files are converted to html
        {"php",  "text/html"}
};


string_t getFileContentType(uri_t pUri) {
    // Look at the last part of the uri for the extension.
    char *c = getFileExtension(pUri);

    if (strcmp(c, "") == 0) {
        // File is extensionless so we return default.
        return stringFromCString("text/plain");
    }

    // Scan through file types.
    for (int i = 0; i < numFileTypes; i++) {
        if (strcmp(fileTypes[i][0], c) == 0) {
            return stringFromCString(fileTypes[i][1]);
        }
    }

    // use plain text by default
    return stringFromCString("text/plain");
}

char *getFileExtension(uri_t uri) {
    string_t lastPart = uri->parts[uri->numParts - 1];

    char *end = charAt(lastPart, stringLength(lastPart) - 1);
    char *c = end;
    // Backup to find the extension
    for (; *c != '.' && c != stringData(lastPart); c--);

    if (*c == '.') {
        return c + 1;
    } else {
        return end;
    }
}