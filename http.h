#pragma once
#include "channel.h"
#include "cache.h"

void handleHttpConnection(int socketFd, channel_t logger, int webRoot, fileCache_t cache);

