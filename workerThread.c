//
// Created by rose on 2/7/19.
//

#include <malloc.h>
#include "workerThread.h"
#include "http.h"

workerParams_t createWorkerParams(channel_t in) {
    workerParams_t ret = malloc(sizeof(struct workerParams));
    ret->inputChannel = in;
    return ret;
}

void destroyWorkerParams(workerParams_t workerParams) {
    free(workerParams);
}

void *httpFileWorker(workerParams_t params) {
    channel_t in = params->inputChannel;
    destroyWorkerParams(params);

    while (true) {
        int* fd;
        if (channelReceive(in, (void **) &fd) == CHANNEL_CLOSED) {
            return NULL;
        }
        handleHttpConnection(*fd);
        free(fd);
    }
}

void createHttpWorkerPool(pthread_t *threads, size_t numThreads, channel_t *inputChannel) {
    channel_t channel = createChannel();

    for (size_t i = 0; i < numThreads; i++) {
        pthread_create(threads + i, NULL, (void *(*)(void *)) &httpFileWorker, createWorkerParams(channel));
    }

    *inputChannel = channel;
}
