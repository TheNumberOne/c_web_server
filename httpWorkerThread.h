#pragma once

#include <pthread.h>
#include "channel.h"
#include "string.h"
#include "cache.h"

struct httpWorkerThreadParams {
    channel_t inputChannel;
    channel_t loggingChannel;
    int workingDirectory;
    fileCache_t cache;
};

typedef struct httpWorkerThreadParams *httpWorkerThreadParams_t;

httpWorkerThreadParams_t
createHttpWorkerParams(channel_t input, channel_t logging, int workingDirectory, fileCache_t cache);

void destroyHttpWorkerParams(httpWorkerThreadParams_t);

void *httpFileWorker(httpWorkerThreadParams_t params);

/**
 * Creates an http worker pool.
 * @param threads The location to store the ids.
 * @param numThreads The number of threads to put in the pool
 * @param inputChannel out parameter for a channel to communicate to worker threads. Use
 * by sending a pointer to the file descriptor of the socket for an incoming connection.
 * The http thread will close the socket and free the location in memory it is stored at once
 * finished processing.
 */
void
createHttpWorkerPool(channel_t logger, fileCache_t cache, int workingDirectory, pthread_t *threads, size_t numThreads,
                     channel_t *inputChannel);


void logMessageWithIp(channel_t logger, int fd, string_t message);

void logMessageWithIpC(channel_t logger, int fd, char *s);