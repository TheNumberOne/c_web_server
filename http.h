#ifndef PROJECT2_HTTP_H
#define PROJECT2_HTTP_H

#include "channel.h"

void handleHttpConnection(int socketFd, channel_t logger);

#endif //PROJECT2_HTTP_H
