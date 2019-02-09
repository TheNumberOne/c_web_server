//
// Created by rose on 2/7/19.
//

#include <malloc.h>
#include <sys/socket.h>

#include "httpWorkerThread.h"
#include "http.h"
#include "loggerWorkerThread.h"

/**
 * Parameters passed to each individual worker thread on startup.
 */
struct httpWorkerThreadParams {
    channel_t inputChannel;
    channel_t loggingChannel;
    fileCache_t cache;
};

typedef struct httpWorkerThreadParams *httpWorkerThreadParams_t;

httpWorkerThreadParams_t
createHttpWorkerParams(channel_t input, channel_t logging, fileCache_t cache);

void destroyHttpWorkerParams(httpWorkerThreadParams_t);

void *httpFileWorker(httpWorkerThreadParams_t params);

httpWorkerThreadParams_t
createHttpWorkerParams(channel_t input, channel_t logging, fileCache_t cache) {
    httpWorkerThreadParams_t ret = malloc(sizeof(struct httpWorkerThreadParams));
    ret->inputChannel = input;
    ret->loggingChannel = logging;
    ret->cache = cache;
    return ret;
}

void destroyHttpWorkerParams(httpWorkerThreadParams_t workerParams) {
    free(workerParams);
}

void *httpFileWorker(httpWorkerThreadParams_t params) {
    // extract params.
    channel_t in = params->inputChannel;
    channel_t logger = params->loggingChannel;
    fileCache_t cache = params->cache;
    destroyHttpWorkerParams(params);

    while (true) {
        int *fd;
        // process input until input channel closes.
        if (channelReceive(in, (void **) &fd) == CHANNEL_CLOSED) {
            return NULL;
        }

        logMessageWithIp(logger, *fd, stringFromCString("Connection received."));

        handleHttpConnection(*fd, logger, cache);
        free(fd);
    }
}

void
createHttpWorkerPool(channel_t logger, fileCache_t cache, pthread_t *threads, size_t numThreads,
                     channel_t *inputChannel) {
    channel_t channel = createChannel();

    for (size_t i = 0; i < numThreads; i++) {
        pthread_create(threads + i, NULL, (void *(*)(void *)) &httpFileWorker,
                       createHttpWorkerParams(channel, logger, cache));
    }

    *inputChannel = channel;
}

