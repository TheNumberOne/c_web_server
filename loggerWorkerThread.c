//
// Created by rose on 2/7/19.
//

#include "httpWorkerThread.h"
#include <fcntl.h>
#include <malloc.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "loggerWorkerThread.h"

struct loggerThreadStartParams {
    int fd;
    channel_t channel;
};
typedef struct loggerThreadStartParams *loggerThreadStartParams_t;

void *loggingThread(loggerThreadStartParams_t params);

void createLoggerThread(string_t path, pthread_t *thread, channel_t *loggingChannel) {
    int fd = open(stringData(path), O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    if (fd == -1) {
        fd = STDOUT_FILENO;
        printf("Unable to open file %s. Using standard output instead.", stringData(path));
    }

    destroyString(path);
    *loggingChannel = createChannel();

    loggerThreadStartParams_t params = malloc(sizeof(struct loggerThreadStartParams));
    params->fd = fd;
    params->channel = *loggingChannel;

    pthread_create(thread, NULL, (void *(*)(void *)) &loggingThread, params);
}


struct tm getCurrentTime() {
    time_t rawTime;
    time(&rawTime);
    struct tm time;
    gmtime_r(&rawTime, &time);
    return time;
}

void *loggingThread(loggerThreadStartParams_t params) {
    int fd = params->fd;
    channel_t channel = params->channel;
    free(params);

    while (true) {
        string_t toLog;
        if (channelReceive(channel, (void **) &toLog) != CHANNEL_OK) break;

        struct tm time = getCurrentTime();
        dprintf(fd, "[%04d-%02d-%02d %02d:%02d:%02d]: %*s\n",
                1900 + time.tm_year,
                time.tm_mon,
                time.tm_mday,
                time.tm_hour,
                time.tm_min,
                time.tm_sec,
                (int) stringLength(toLog),
                stringData(toLog));

        destroyString(toLog);
    }

    close(fd);
    return NULL;
}

void logMessage(channel_t loggingChannel, string_t message) {
    channelSend(loggingChannel, message);
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

void logMessageWithIpC(channel_t logger, int fd, char *s) {
    logMessageWithIp(logger, fd, stringFromCString(s));
}