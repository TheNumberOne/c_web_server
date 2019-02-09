#pragma once

/**
 * Heavily based off of https://github.com/tylertreat/chan
 */

#include <zconf.h>
#include <stdbool.h>

/**
 * A channel object. Basically allows for sychronous communication across threads.
 * Readers and writers (consumers and producers) block until their counterpart receives the message.
 * Doesn't use a queue or buffer. So speed can be improved by adding those.
 */
struct channel;


typedef enum {
    CHANNEL_OK,
    CHANNEL_CLOSED
} channel_result_t;

typedef struct channel *channel_t;

/**
 * Creates a channel object.
 */
channel_t createChannel();

/**
 * Destroys a channel object. Make sure no threads are currently sending or receiving.
 */
void destroyChannel(channel_t self);

/**
 * Closes the channel. All threads that try to send or receive data will get notified of its closure.
 */
void closeChannel(channel_t self);

/**
 * Sends data through the channel. Will block until another thread retrieves the data,
 * @param data A pointer to the data to send.
 * @return If the channel is closed or if sending the data was successful.
 */
channel_result_t channelSend(channel_t self, void *data);

/**
 * Receives data from the channel.
 * @param data The location that this is pointing to is set with a pointer to the data passed by send.
 * @return Whether the channel is closed.
 */
channel_result_t channelReceive(channel_t self, void **data);