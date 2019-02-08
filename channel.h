#pragma once

#include <zconf.h>
#include <stdbool.h>

struct channel {

    pthread_mutex_t readMutex;
    pthread_mutex_t writeMutex;
    void *data;

    pthread_mutex_t channelMutex;
    pthread_cond_t readSignaler;
    pthread_cond_t writeSignaler;

    bool closed;
    bool readWaiting;
    bool writeWaiting;
};


typedef enum {
    CHANNEL_OK,
    CHANNEL_CLOSED
} channel_result_t;

typedef struct channel *channel_t;

channel_t createChannel();

void destroyChannel(channel_t self);

void closeChannel(channel_t self);

channel_result_t channelSend(channel_t self, void *data);

channel_result_t channelReceive(channel_t self, void **data);