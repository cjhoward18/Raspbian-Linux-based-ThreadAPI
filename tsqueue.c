#include "tsqueue.h"

/* tsqueue.c adapted from OSPP TSQueue.cc */
/* Thread-safe queue implementation. */

/* Initialize the queue to empty
   and the lock to free.
*/
void
tsqueue_init(struct tsqueue *q) {
    q->front = q->next_empty = 0;
    lock_init(&q->lk);
}

/* Try to insert an item. If the queue is
   full, return false; otherwise return true.
*/
bool 
tsqueue_try_insert(struct tsqueue *q, int item) {
    bool success = false;

    lock_acquire(&q->lk);
    if ((q->next_empty - q->front) < MAX) {
        q->items[q->next_empty % MAX] = item;
        q->next_empty++;
        success = true;
    }
    lock_release(&q->lk);
    return success;
}

/* Try to remove an item. If the queue is 
   empty, return false; otherwise return true.
*/
bool 
tsqueue_try_remove(struct tsqueue *q, int *item) {
    bool success = false;

    lock_acquire(&q->lk);
    if (q->front < q->next_empty) {
        *item = q->items[q->front % MAX];
        q->front++;
        success = true;
    }
    lock_release(&q->lk);
    return success;
}
