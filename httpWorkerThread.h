#pragma once

#include <pthread.h>
#include "channel.h"
#include "string.h"
#include "cache.h"


/**
 * Creates an http worker pool. Takes ownership of none of the params. Ownership of created
 * inputChannel is passed to caller.
 * @param cache The file cache that files are retrieved from.
 * @param threads The location to store the ids.
 * @param numThreads The number of threads to put in the pool
 * @param inputChannel out parameter for a channel to communicate to worker threads. Use
 * by sending a pointer to the file descriptor of the socket for an incoming connection.
 * The http thread will close the socket and free the location in memory it is stored at once
 * finished processing.
 */
void
createHttpWorkerPool(channel_t logger, fileCache_t cache, pthread_t *threads, size_t numThreads,
                     channel_t *inputChannel);


