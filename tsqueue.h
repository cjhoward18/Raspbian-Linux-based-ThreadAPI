#ifndef _TSQUEUE_H_
#define _TSQUEUE_H_
#include <stdbool.h>

#include "thread.h"
#include "lock.h"

/* tsqueue.h adapted from OSPP TSQueue.h */
/* Thread-safe queue interface */

#define MAX 10

struct tsqueue {
    /* Synchronization variables */
    struct lock lk;

    /* State variables */
    int items[MAX];
    int front;
    int next_empty;
};

void tsqueue_init(struct tsqueue *q);
bool tsqueue_try_insert(struct tsqueue *q, int item);
bool tsqueue_try_remove(struct tsqueue *q, int *item);

#endif /* _TSQUEUE_H_ */
