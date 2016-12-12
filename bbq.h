#ifndef _BBQ_H_
#define _BBQ_H_

#include "thread.h"
#include "lock.h"
#include "cond.h"

/* bbq.h adapted from OSPP BBQ.h */
/* Thread-safe blocking queue. */

#define MAX 10

struct bbq {
    /*  Synchronization variables */
    struct lock lk;
    struct cond item_added_cv;
    struct cond item_removed_cv;

    /* State variables */
    int items[MAX];
    int front;
    int next_empty;
};

void bbq_init(struct bbq *q);
void bbq_insert(struct bbq *q, int item);
int bbq_remove(struct bbq *q);

#endif /* _BBQ_H_ */
