#include <memory.h>
#include "uri.h"
#include "mimeTypes.h"


const int numFileTypes = 6;

/**
 * The different file types we know about.
 */
const char fileTypes[6][2][16] = {
        {"html", "text/html"},
        {"js",   "text/javascript"},
        {"css",  "text/css"},
        {"png",  "image/png"},
        {"jpeg", "image/jpeg"},
        {"jpg",  "image/jpeg"},
};

string_t getFileContentType(uri_t pUri) {
    // Look at the last part of the uri for the extension.
    string_t lastPart = pUri->parts[pUri->numParts - 1];

    char *c = charAt(lastPart, stringLength(lastPart) - 1);
    // Backup to find the extension
    for (; *c != '.' && c != stringData(lastPart); c--);

    if (*c == '.') {
        c++;
    } else {
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