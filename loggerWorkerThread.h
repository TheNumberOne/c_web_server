#pragma once

#include <pthread.h>
#include "channel.h"
#include "string.h"

void createLoggerThread(string_t path, pthread_t *thread, channel_t *loggingChannel);

void logMessage(channel_t loggingChannel, string_t message);