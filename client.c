#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include "string.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,254,stdin);
    size_t bufferLen = strlen(buffer);
    if (bufferLen > 0 && buffer[bufferLen - 1] == '\n' && buffer[bufferLen - 2] != '\r') {
        buffer[bufferLen - 1] = '\r';
        buffer[bufferLen] = '\n';
        buffer[bufferLen + 1] = 0;
    }
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,256);
    string_t input = createString();
    char c;
    while (read(sockfd, &c, 1) > 0) {
        append(input, c);
    }
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n",stringData(input));

    destroyString(input);
    close(sockfd);
    return 0;
}