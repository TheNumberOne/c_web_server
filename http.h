#pragma once

#include "channel.h"
#include "cache.h"

/**
 * Handles an http connection
 * @param socketFd The socket of the incoming connection. This socket is closed once this function returns.
 * @param logger The logger. Not taken ownership.
 * @param cache The cache. Not taken ownership.
 */
void handleHttpConnection(int socketFd, channel_t logger, fileCache_t cache);
