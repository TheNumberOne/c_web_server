#pragma once
#include "channel.h"

void handleHttpConnection(int socketFd, channel_t logger, int webRoot);

