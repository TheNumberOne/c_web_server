#include <stdbool.h>
#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include "errorHandling.h"

const int DEFAULT_ERR_STR_SIZE = 1024;

struct result errorFromConstString(const char *e) {
    struct result err;
    err.ok = false;
    err.error = (char *) e;
    err.free = false;
    return err;
}


struct result okResult() {
    struct result ok;
    ok.ok = true;
    ok.error = NULL;
    ok.free = false;
    return ok;
}

struct result errorFromErrno() {
    int e = errno;

    struct result err;
    err.ok = false;
    err.free = true;

    size_t errSize = sizeof(char) * DEFAULT_ERR_STR_SIZE;
    err.error = malloc(errSize);

    // Double the buffer until it fits the error.
    while (true) {
        // Check to see if malloc or realloc failed
        if (err.error == NULL) {
            return errorFromConstString("Out of memory");
        }

        //Retrieve error string.
        int r = strerror_r(e, err.error, errSize);

        //Earlier versions of glib would return the error in errno and return -1.
        r = (r == -1 ? errno : r);
        if (r == 0) {
            return err;
        } else if (r == EINVAL) {
            e = r;
        } else if (r == ERANGE) {
            errSize *= 2;
            err.error = realloc(err.error, errSize);
        }
    }
}

void cleanupError(struct result err) {
    if (err.free) free(err.error);
}

void exitIfError(struct result result) {
    if (result.ok) return;

    fprintf(stderr, "%s\n", result.error);
    cleanupError(result);
    exit(1);
}
