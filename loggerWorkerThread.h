#pragma once

#include <pthread.h>
#include "channel.h"
#include "string.h"

/**
 * Creates a logging thread that logs to the specified location
 * @param path Appends logs to the file at the specified path. Uses stdout if it
 * is not a valid path. Takes ownership.
 * @param thread The location to store the created thread. Ownership passed to caller.
 * @param loggingChannel The location to store the channel feeding this logger thread
 * commands. Ownership passed to caller.
 */
void createLoggerThread(string_t path, pthread_t *thread, channel_t *loggingChannel);

/**
 * Sends the message to the logging thread.
 * @param loggingChannel The channel to the logging thread. Ownership not taken
 * @param message the message to send. Ownership taken.
 */
void logMessage(channel_t loggingChannel, string_t message);

/**
 * Sends the message to the logging thread
 * @param logger the channel to the logging thread. Ownership not taken
 * @param fd A file descriptor for a network connection. Ownership not taken.
 * @param message Ownership taken.
 */
void logMessageWithIp(channel_t logger, int fd, string_t message);

/**
 * Sends the message to the logging thread.
 * @param logger The channel to the logging thread. Ownership not taken
 * @param fd A file descriptor for a network connection. Ownership not taken.
 * @param s The message. Ownership not taken.
 */
void logMessageWithIpC(channel_t logger, int fd, char *s);