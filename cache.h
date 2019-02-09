#pragma once

#include "string.h"
#include "channel.h"

struct fileCache;
typedef struct fileCache *fileCache_t;

/**
 * Creates a file cache. Does not take ownership of any of the parameters.
 */
fileCache_t createFileCache(size_t maxBytes, int workingDirectory, channel_t logger);

void destroyFileCache(fileCache_t self);

/**
 * Retrieves the contents of the file with the specified path relative to working
 * directory. Multi-threaded safe.
 * @param path the relative path to the file. This does not take ownership of path.
 * @return the contents of the file. Ownership is passed to the calling thread. Returns different
 * object for each evocation. NULL if the file could not be found.
 */
string_t fileCacheGetFile(fileCache_t cache, const char *path);
