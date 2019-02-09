#include <malloc.h>
#include <pthread.h>
#include <memory.h>
#include <fcntl.h>
#include <assert.h>
#include "cache.h"
#include "loggerWorkerThread.h"

// File cache implemented as singly linked list
struct fileCacheEntry {
    /**
     * The path to the file.
     */
    char *path;

    /**
     * The contents of the file.
     */
    string_t file;

    /**
     * The size of the file.
     */
    size_t size;

    /**
     * Pointer to next file cache entry
     */
    struct fileCacheEntry *next;
};

typedef struct fileCacheEntry *fileCacheEntry_t;

struct fileCache {
    /**
     * Used to control access to inside of file cache.
     */
    pthread_mutex_t mutex;

    /**
     * Maximum bytes allowed in the file cache.
     */
    size_t maxBytes;

    /**
     * Working directory used to open files.
     */
    int workingDirectory;

    /**
     * Current number of bytes in the file cache.
     */
    size_t currentBytes;

    /**
     * Pointer to first cache entry.
     */
    fileCacheEntry_t entryList;

    /**
     * The logger used
     */
    channel_t logger;
};

fileCache_t createFileCache(size_t maxBytes, int workingDirectory, channel_t logger) {
    fileCache_t result = malloc(sizeof(struct fileCache));
    pthread_mutex_init(&result->mutex, NULL);
    result->maxBytes = maxBytes;
    result->workingDirectory = workingDirectory;
    result->currentBytes = 0;
    result->entryList = NULL;
    result->logger = logger;
    return result;
}

void destroyFileCache(fileCache_t self) {
    fileCacheEntry_t f = self->entryList;

    // Destroy the file cache chain
    while (f != NULL) {
        fileCacheEntry_t next = f->next;
        free(f->path);
        destroyString(f->file);
        free(f);
        f = next;
    }
    pthread_mutex_destroy(&self->mutex);
    free(self);
}

string_t fileCacheGetFile(fileCache_t cache, const char *path) {
    string_t filename = stringFromCString(path);

    pthread_mutex_lock(&cache->mutex);

    // First search for the file name in the cache.
    // previous is the pointer to the location pointing to f
    // so we can change it if we move f.
    fileCacheEntry_t *prev = &cache->entryList;
    fileCacheEntry_t f = cache->entryList;
    for (; f != NULL; f = *(prev = &f->next)) {
        if (strcmp(path, f->path) == 0) break;
    }

    // Found the file!
    if (f != NULL) {

        // Remove it from the list
        *prev = f->next;

        // Insert it at front
        f->next = cache->entryList;
        cache->entryList = f;

        // Copy the contents
        string_t copied = stringCopy(f->file);
        pthread_mutex_unlock(&cache->mutex);

        string_t toLog = stringFromCString("[cache] found cached file: ");
        plusEqual(toLog, filename);
        logMessage(cache->logger, toLog);
        destroyString(filename);

        return copied;
    }

    // unlock the mutex while reading the file
    pthread_mutex_unlock(&cache->mutex);

    string_t toLog = stringFromCString("[cache] Failed to find cached file. Reading from file system: ");
    plusEqual(toLog, filename);
    logMessage(cache->logger, toLog);

    int fd = openat(cache->workingDirectory, path, O_RDONLY);

    // Didn't find the file.
    if (fd < 0) {
        toLog = stringFromCString("[cache] File not found on file system: ");
        plusEqual(toLog, filename);
        logMessage(cache->logger, toLog);
        destroyString(filename);
        return NULL;
    }

    // Read file
    string_t contents = createString();
    char c;
    while (read(fd, &c, 1) > 0) {
        append(contents, c);
    }

    toLog = stringFromCString("[cache] File read from file system: ");
    plusEqual(toLog, filename);
    logMessage(cache->logger, toLog);

    // Lock cache to put the file into the cache
    pthread_mutex_lock(&cache->mutex);

    // Don't cache HUUUUUUGE files
    if (stringLength(contents) > cache->maxBytes) {
        pthread_mutex_unlock(&cache->mutex);

        toLog = stringFromCString("[cache] File too big to cache: ");
        plusEqual(toLog, filename);
        logMessage(cache->logger, toLog);
        destroyString(filename);

        return contents;
    }

    //Check to see if it's been inserted while we were reading the file.
    for (f = cache->entryList; f != NULL; f = f->next) {
        if (strcmp(path, f->path) == 0) {
            // inserted!
            pthread_mutex_unlock(&cache->mutex);

            toLog = stringFromCString("[cache] file was cached by another thread: ");
            plusEqual(toLog, filename);
            logMessage(cache->logger, toLog);
            destroyString(filename);

            return contents;
        }
    }

    // Time to insert it!

    size_t newBytes = cache->currentBytes + stringLength(contents);
    size_t cacheEntriesRemoved = 0;
    // Remove stuff from the cache if necessary
    if (newBytes > cache->maxBytes) {
        size_t bytesSoFar = stringLength(contents);
        // first find the first entry to remove.
        for (f = *(prev = &cache->entryList); f != NULL; f = *(prev = &f->next)) {
            if (bytesSoFar + f->size > cache->maxBytes) {
                break;
            }
            bytesSoFar += f->size;
        }

        // Remove the last few entries.
        *prev = NULL;
        while (f != NULL) {
            fileCacheEntry_t next = f->next;
            free(f->path);
            destroyString(f->file);
            cache->currentBytes -= f->size;
            free(f);
            f = next;
            cacheEntriesRemoved++;
        }
    }
    assert(cache->currentBytes + stringLength(contents) <= cache->maxBytes);

    // Add the new cache entry
    fileCacheEntry_t newEntry = malloc(sizeof(struct fileCacheEntry));
    newEntry->size = stringLength(contents);
    newEntry->path = malloc(strlen(path) + 1);
    strcpy(newEntry->path, path);
    newEntry->file = stringCopy(contents);
    newEntry->next = cache->entryList;
    cache->entryList = newEntry;
    cache->currentBytes += stringLength(contents);

    pthread_mutex_unlock(&cache->mutex);

    // Logging >.<
    // should add some util functions to make it easier to log.
    toLog = stringFromCString("[cache] Removed ");
    string_t i = stringFromInt((int) cacheEntriesRemoved);
    plusEqual(toLog, i);
    destroyString(i);
    i = stringFromCString(" entries from cache. Added ");
    plusEqual(toLog, i);
    destroyString(i);
    plusEqual(toLog, filename);
    logMessage(cache->logger, toLog);
    destroyString(filename);

    return contents;
}
