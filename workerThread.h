
#include <pthread.h>
#include "channel.h"

struct workerParams {
    channel_t inputChannel;
};

typedef struct workerParams *workerParams_t;

workerParams_t createWorkerParams(channel_t);

void destroyWorkerParams(workerParams_t);

void *httpFileWorker(workerParams_t params);

void createHttpWorkerPool(pthread_t *threads, size_t numThreads, channel_t *inputChannel);