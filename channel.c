//
// Created by rose on 2/7/19.
//

#include <malloc.h>
#include <pthread.h>
#include "channel.h"

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
    pthread_mutex_lock(&self->writeMutex);
    pthread_mutex_lock(&self->channelMutex);

    if (self->closed) {
        pthread_mutex_unlock(&self->channelMutex);
        pthread_mutex_unlock(&self->writeMutex);
        return CHANNEL_CLOSED;
    }


    self->data = data;
    self->writeWaiting = true;

    // Signal to readers that they can read now.
    if (self->readWaiting) {
        pthread_cond_signal(&self->readSignaler);
    }

    pthread_cond_wait(&self->writeSignaler, &self->channelMutex);

    pthread_mutex_unlock(&self->channelMutex);
    pthread_mutex_unlock(&self->writeMutex);

    return CHANNEL_OK;
}

channel_result_t channelReceive(channel_t self, void **data) {
    pthread_mutex_lock(&self->readMutex);
    pthread_mutex_lock(&self->channelMutex);

    if (!self->closed && !self->writeWaiting) {
        self->readWaiting = true;
        pthread_cond_wait(&self->readSignaler, &self->channelMutex);
        self->readWaiting = false;
    }

    if (self->closed) {
        pthread_mutex_unlock(&self->channelMutex);
        pthread_mutex_unlock(&self->readMutex);
        return CHANNEL_CLOSED;
    }

    *data = self->data;

    // This line has to be here rather than in send because if it is in send,
    // it's possible for this function call to end before the writing thread has been awoken in which
    // case it's possible another thread can come in to read it.
    self->writeWaiting = false;

    // Tell writer it can go now.
    pthread_cond_signal(&self->writeSignaler);

    pthread_mutex_unlock(&self->channelMutex);
    pthread_mutex_unlock(&self->readMutex);

    return CHANNEL_OK;
}
