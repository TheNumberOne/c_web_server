/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include "errorHandling.h"
#include "http.h"
#include "channel.h"
#include "httpWorkerThread.h"
#include "loggerWorkerThread.h"

struct result parsePortNum(const char *arg, uint16_t *ret);

struct result connectToSocket(uint16_t portNum, int *socketFd);

struct result acceptConnection(int socketFd, int *newSocketFd);

struct result acceptConnection(int socketFd, int *newSocketFd) {
    struct sockaddr_in cliAddr;
    uint cliLen = sizeof(cliAddr);

    *newSocketFd = accept(socketFd, (struct sockaddr *) &cliAddr, &cliLen);

    if (*newSocketFd < 0) {
        return errorFromErrno();
    }

    return okResult();
}

struct result connectToSocket(uint16_t portNum, int *socketFd) {
    *socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socketFd < 0) {
        return errorFromErrno();
    }

    struct sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portNum);

    if (bind(*socketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        return errorFromErrno();
    }

    if (listen(*socketFd, 5) < 0) {
        return errorFromErrno();
    }

    return okResult();
}

struct result parsePortNum(const char *arg, uint16_t *ret) {
    char *endChar;
    errno = 0;
    long portNoLong = strtol(arg, &endChar, 10);

    if (errno != 0) return errorFromErrno();
    if (endChar == arg) return errorFromConstString("Invalid port");
    if (portNoLong < 0) return errorFromConstString("Port must not be negative");
    if (portNoLong > UINT16_MAX) return errorFromConstString("Port must not be too big");

    *ret = (uint16_t) portNoLong;

    return okResult();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        dprintf(STDERR_FILENO, "ERROR, no settings file provided\n");
        return 1;
    }

    int f = open(argv[1], O_RDONLY);
    if (f < 0) {
        dprintf(STDERR_FILENO, "ERROR unable to open file: %s\n", argv[1]);
        return 1;
    }

    string_t file = createString();

    char c;
    while (read(f, &c, 1) > 0) {
        append(file, c);
    }
    close(f);


    uint16_t port;
    char* directoryPath;
    char* logFile;

    // %m modifier not matched by clang diagnostics
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
    if (sscanf(stringData(file), " port = %hu webroot = %m[^\n] log = %m[^\n]", &port, &directoryPath, &logFile)
        < 3) {
        dprintf(STDERR_FILENO, "ERROR, invalid settings file. Format:\n");
        dprintf(STDERR_FILENO, "port=<portnum>\n");
        dprintf(STDERR_FILENO, "webroot=<path_to_web_root>\n");
        dprintf(STDERR_FILENO, "log=<path_to_log_file>\n");
        destroyString(file);
        return 1;
    }
#pragma clang diagnostic pop

    destroyString(file);

    int webRoot = open(directoryPath, O_DIRECTORY);
    if (webRoot == -1) {
        dprintf(STDERR_FILENO, "Invalid web root: %s", directoryPath);
        free(directoryPath);
        free(logFile);
        return 1;
    }
    free(directoryPath);

    channel_t logging;
    pthread_t loggingThread;
    createLoggerThread(stringFromCString(logFile), &loggingThread, &logging);
    free(logFile);

    channel_t httpThreadPool;
    pthread_t threads[20];
    createHttpWorkerPool(logging, webRoot, threads, 20, &httpThreadPool);

    int sockFd;
    struct result err = connectToSocket(port, &sockFd);

    if (!err.ok) {
        closeChannel(httpThreadPool);
        closeChannel(logging);
        for (size_t i = 0; i < 20; i++) {
            pthread_join(threads[i], NULL);
        }
        pthread_join(loggingThread, NULL);
        destroyChannel(httpThreadPool);
        destroyChannel(logging);
        close(webRoot);
        exitIfError(err);
    }

    while (true) {
        int newSockFd;
        err = acceptConnection(sockFd, &newSockFd);
        if (!err.ok) break;

        int* command = malloc(sizeof(int));
        *command = newSockFd;

        // Cast int to void pointer because the other side only wants the int.
        if (channelSend(httpThreadPool, command) == CHANNEL_CLOSED) {
            break;
        }
    }

    close(sockFd);
    exitIfError(err);
    return 0;
}

