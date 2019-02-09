#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "string.h"

string_t fixLineFeeds(char buffer[256], size_t n);

void *pipeToOutput(void *voidFd);

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          (size_t) server->h_length);
    serv_addr.sin_port = htons((uint16_t) portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    pthread_t t;
    pthread_create(&t, NULL, &pipeToOutput, (void *) sockfd);

    char buffer[256];
    while (true) {
        ssize_t ret = read(STDIN_FILENO, &buffer, 256);
        if (ret < 0) error("ERROR reading");
        if (ret == 0) break;

        string_t s = fixLineFeeds(buffer, (size_t) ret);

        size_t totalWritten = 0;
        while (stringLength(s) - totalWritten > 0) {
            ssize_t written = write(sockfd, stringData(s) + totalWritten, stringLength(s) - totalWritten);
            if (written < 0) error("ERROR writing");
            totalWritten += written;
        }

        destroyString(s);
    }
    close(sockfd);
    return 0;
}

string_t fixLineFeeds(char *buffer, size_t n) {
    string_t s = createString();
    for (size_t i = 0; i < n; i++) {
        if (buffer[i] == '\n' && (i == 0 || buffer[i - 1] != '\r')) {
            append(s, '\r');
        }
        append(s, buffer[i]);
    }
    return s;
}

void *pipeToOutput(void *voidFd) {
    int fd = (int) voidFd;
    char buffer[256];
    while (true) {
        ssize_t ret = read(fd, &buffer, 256);
        if (ret < 0) error("ERROR reading");
        if (ret == 0) break;

        while (ret > 0) {
            ssize_t written = write(STDOUT_FILENO, buffer, (size_t) ret);
            if (written < 0) error("ERROR writing");
            ret -= written;
        }
    }
    return NULL;
}