//
// Created by rose on 2/7/19.
//

#include <malloc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "httpWorkerThread.h"
#include "http.h"
#include "loggerWorkerThread.h"

httpWorkerThreadParams_t createHttpWorkerParams(channel_t input, channel_t logging) {
    httpWorkerThreadParams_t ret = malloc(sizeof(struct httpWorkerThreadParams));
    ret->inputChannel = input;
    ret->loggingChannel = logging;
    return ret;
}

void destroyHttpWorkerParams(httpWorkerThreadParams_t workerParams) {
    free(workerParams);
}

void logMessageWithIp(channel_t logger, int fd, string_t message) {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    getpeername(fd, (struct sockaddr *) &addr, &addrLen);

    string_t s = stringFromCString(inet_ntoa(addr.sin_addr));
    char buffer[256];
    snprintf(buffer, 256, "[%s]: %s", stringData(s), stringData(message));

    moveString(s, stringFromCString(buffer));
    logMessage(logger, s);

    destroyString(message);
}

void *httpFileWorker(httpWorkerThreadParams_t params) {
    channel_t in = params->inputChannel;
    channel_t logger = params->loggingChannel;
    destroyHttpWorkerParams(params);

    while (true) {
        int *fd;
        if (channelReceive(in, (void **) &fd) == CHANNEL_CLOSED) {
            return NULL;
        }

        logMessageWithIp(logger, *fd, stringFromCString("Connection received."));

        handleHttpConnection(*fd, logger);
        free(fd);
    }
}

void createHttpWorkerPool(channel_t logger, pthread_t *threads, size_t numThreads, channel_t *inputChannel) {
    channel_t channel = createChannel();

    for (size_t i = 0; i < numThreads; i++) {
        pthread_create(threads + i, NULL, (void *(*)(void *)) &httpFileWorker, createHttpWorkerParams(channel, logger));
    }

    *inputChannel = channel;
}

void logMessageWithIpC(channel_t logger, int fd, char *s) {
    logMessageWithIp(logger, fd, stringFromCString(s));
}
