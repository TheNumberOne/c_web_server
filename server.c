/**
 * The main file in this program.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include "errorHandling.h"
#include "http.h"
#include "httpWorkerThread.h"
#include "loggerWorkerThread.h"


struct result connectToSocket(uint16_t portNum, int *socketFd);

struct result acceptConnection(int socketFd, int *newSocketFd);

/**
 * Accepts a connection from the socket and stores it in the new socket
 * @param socketFd not taken ownership
 * @param newSocketFd ownership given to caller.
 */
struct result acceptConnection(int socketFd, int *newSocketFd) {
    struct sockaddr_in cliAddr;
    uint cliLen = sizeof(cliAddr);

    *newSocketFd = accept(socketFd, (struct sockaddr *) &cliAddr, &cliLen);

    if (*newSocketFd < 0) {
        return errorFromErrno();
    }

    return okResult();
}

/**
 * Connects to the specified socket.
 *
 * @param portNum the port
 * @param socketFd A pointer to the location to return to. Ownership passed to caller.
 */
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        dprintf(STDERR_FILENO, "ERROR, no settings file provided\n");
        return 1;
    }

    // open settings file
    int f = open(argv[1], O_RDONLY);
    if (f < 0) {
        dprintf(STDERR_FILENO, "ERROR unable to open file: %s\n", argv[1]);
        return 1;
    }

    // read settings file
    string_t file = createString();
    char c;
    while (read(f, &c, 1) > 0) {
        append(file, c);
    }
    close(f);

    //parse settings file.
    uint16_t port;
    char *directoryPath;
    char *logFile;
    unsigned int numWorkerThreads;
    size_t cacheSize;

    // %m modifier not matched by clang diagnostics
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
    if (sscanf(stringData(file), " port = %hu webroot = %m[^\n] log = %m[^\n] num_worker_threads = %u cache_size = %lu",
               &port,
               &directoryPath, &logFile, &numWorkerThreads, &cacheSize)
        < 5) {
#pragma clang diagnostic pop
        dprintf(STDERR_FILENO, "ERROR, invalid settings file. Format (must be in specified order):\n");
        dprintf(STDERR_FILENO, "port=<portnum>\n");
        dprintf(STDERR_FILENO, "webroot=<path_to_web_root>\n");
        dprintf(STDERR_FILENO, "log=<path_to_log_file>\n");
        dprintf(STDERR_FILENO, "num_worker_threads=<n>\n");
        dprintf(STDERR_FILENO, "cache_size=<num_bytes>\n");
        destroyString(file);
        return 1;
    }
    destroyString(file);

    // open the webroot
    int webRoot = open(directoryPath, O_DIRECTORY);
    if (webRoot == -1) {
        dprintf(STDERR_FILENO, "Invalid web root: %s\n", directoryPath);
        free(directoryPath);
        free(logFile);
        return 1;
    }
    free(directoryPath);

    // allocate room for threads
    // Add one for logging thread.
    int totalThreads = numWorkerThreads + 1;
    pthread_t *threads = malloc(totalThreads * sizeof(pthread_t));

    // allocate room for channels.
    const int LOGGING_CHANNEL = 0;
    const int HTTP_POOL_CHANNEL = 1;
    int numChannels = 2;
    channel_t *channels = malloc(numChannels * sizeof(channel_t));

    // create the logging file
    createLoggerThread(stringFromCString(logFile), threads, &channels[LOGGING_CHANNEL]);
    free(logFile);

    // create the file cache.
    fileCache_t cache = createFileCache(cacheSize, webRoot, channels[LOGGING_CHANNEL]);

    // create the thread pool
    createHttpWorkerPool(channels[LOGGING_CHANNEL], cache, threads + 1, numWorkerThreads,
                         &channels[HTTP_POOL_CHANNEL]);

    // connect to the socket.
    int sockFd;
    struct result err = connectToSocket(port, &sockFd);

    // close everything if connection failed
    if (!err.ok) {
        for (int i = 0; i < numChannels; i++) {
            closeChannel(channels[i]);
        }
        for (size_t i = 0; i < totalThreads; i++) {
            pthread_join(threads[i], NULL);
        }
        for (int i = 0; i < numChannels; i++) {
            destroyChannel(channels[i]);
        }
        destroyFileCache(cache);
        free(channels);
        free(threads);
        close(webRoot);
        exitIfError(err);
    }

    // accept incoming connections and immediately pass them to other threads.
    while (true) {
        int newSockFd;
        err = acceptConnection(sockFd, &newSockFd);
        if (!err.ok) break;

        int *command = malloc(sizeof(int));
        *command = newSockFd;

        // Cast int to void pointer because the other side only wants the int.
        if (channelSend(channels[HTTP_POOL_CHANNEL], command) == CHANNEL_CLOSED) {
            break;
        }
    }

    close(sockFd);
    exitIfError(err);
    return 0;
}

