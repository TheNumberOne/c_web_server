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
#include "errorHandling.h"
#include "http.h"

struct result parsePortNum(const char *arg, uint16_t *ret);

struct result connectToSocket(uint16_t portNum, int *socketFd);

struct result acceptConnection(int socketFd, int *newSocketFd);

struct result handleConnection(int socketFd, bool *exit);

struct result handleConnection(int socketFd, bool *exit) {
    struct result err = okResult();

    handleHttpConnection(socketFd);
}

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
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    uint16_t portNum;
    exitIfError(parsePortNum(argv[1], &portNum));

    int sockFd;
    exitIfError(connectToSocket(portNum, &sockFd));

    struct result err = okResult();

    bool exit = false;
    while (!exit) {
        int newSockFd;
        err = acceptConnection(sockFd, &newSockFd);
        if (!err.ok) goto cleanupSockFd;

        err = handleConnection(newSockFd, &exit);
        if (!err.ok) goto cleanupSockFd;
    }

    cleanupSockFd:
    close(sockFd);
    exitIfError(err);
    return 0;
}

