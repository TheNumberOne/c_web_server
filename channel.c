#include <malloc.h>
#include <pthread.h>
#include "channel.h"

struct channel {
    /**
     * To prevent more than one thread reading at a time.
     */
    pthread_mutex_t readMutex;

    /**
     * To prevent more than one thread writing at a time.
     */
    pthread_mutex_t writeMutex;

    /**
     * The data that is handed off between threads.
     */
    void *data;

    /**
     * Mutex to control access to internal state of channel
     */
    pthread_mutex_t channelMutex;

    /**
     * Used to signal waiting readers.
     */
    pthread_cond_t readSignaler;

    /**
     * Used to signal waiting writers.
     */
    pthread_cond_t writeSignaler;

    /**
     * Indicates if this channel is closed or not.
     */
    bool closed;

    /**
     * Indicates if a consumer is waiting for a message to be given to it.
     */
    bool readWaiting;

    /**
     * Indicates if a producer is waiting for producer.
     */
    bool writeWaiting;
};

channel_t createChannel() {
    channel_t channel = malloc(sizeof(struct channel));

    pthread_mutex_init(&channel->channelMutex, NULL);
    pthread_mutex_init(&channel->readMutex, NULL);
    pthread_mutex_init(&channel->writeMutex, NULL);
    pthread_cond_init(&channel->readSignaler, NULL);
    pthread_cond_init(&channel->writeSignaler, NULL);

    channel->data = NULL;
    channel->closed = false;
    channel->readWaiting = false;
    channel->writeWaiting = false;

    return channel;
}

void destroyChannel(channel_t self) {
    pthread_mutex_destroy(&self->channelMutex);
    pthread_mutex_destroy(&self->readMutex);
    pthread_mutex_destroy(&self->writeMutex);
    pthread_cond_destroy(&self->readSignaler);
    pthread_cond_destroy(&self->writeSignaler);
    free(self);
}


void closeChannel(channel_t self) {
    pthread_mutex_lock(&self->channelMutex);
    if (!self->closed) {
        self->closed = true;
        pthread_cond_broadcast(&self->writeSignaler);
        pthread_cond_broadcast(&self->readSignaler);
    }
    pthread_mutex_unlock(&self->channelMutex);
}

channel_result_t channelSend(channel_t self, void *data) {
    // Order matters. We don't want a thread to lock the channel
    // and prevent a thread waiting for a reader from locking the channel
    // when it wakes.
    pthread_mutex_lock(&self->writeMutex);
    pthread_mutex_lock(&self->channelMutex);

    if (self->closed) {
        pthread_mutex_unlock(&self->channelMutex);
        pthread_mutex_unlock(&self->writeMutex);
        return CHANNEL_CLOSED;
    }

    // Set data
    self->data = data;
    self->writeWaiting = true;

    // Signal to readers that they can read now.
    if (self->readWaiting) {
        pthread_cond_signal(&self->readSignaler);
    }

    // Wait for reader
    pthread_cond_wait(&self->writeSignaler, &self->channelMutex);

    // Reader sets writeWaiting to false so we don't have to.

    pthread_mutex_unlock(&self->channelMutex);
    pthread_mutex_unlock(&self->writeMutex);

    return CHANNEL_OK;
}

channel_result_t channelReceive(channel_t self, void **data) {
    // Order matters here too
    pthread_mutex_lock(&self->readMutex);
    pthread_mutex_lock(&self->channelMutex);

    if (!self->closed && !self->writeWaiting) {
        self->readWaiting = true;
        // Wait for a producer
        pthread_cond_wait(&self->readSignaler, &self->channelMutex);
        self->readWaiting = false;
    }

    if (self->closed) {
        pthread_mutex_unlock(&self->channelMutex);
        pthread_mutex_unlock(&self->readMutex);
        return CHANNEL_CLOSED;
    }

    // retrieve data
    *data = self->data;

    // This line has to be here rather than in send because if it is in send,
    // it's possible for this function call to end before the writing thread has been awoken in which
    // case it's possible for another thread can come in to read the data before this is set to false.
    self->writeWaiting = false;

    // Tell writer it can go now.
    pthread_cond_signal(&self->writeSignaler);

    pthread_mutex_unlock(&self->channelMutex);
    pthread_mutex_unlock(&self->readMutex);

    return CHANNEL_OK;
}
