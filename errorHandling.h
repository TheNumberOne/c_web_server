#pragma once

/**
 * Only really did errors this way at first.
 * Ended up switching to enums or getting away from them entirely because
 * they're so bothersome to handle in c.
 */

/**
 * Represents the result of a function
 */
struct result {
    /**
     * If the function was successful.
     */
    bool ok;

    /**
     * A c string pointing to the error.
     */
    char *error;

    /**
     * Whether error needs to be freed.
     */
    bool free;
};

void cleanupError(struct result err);

void exitIfError(struct result result);

struct result okResult();

struct result errorFromConstString(const char *e);

struct result errorFromErrno();