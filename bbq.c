#include <assert.h>
#include "thread.h"
#include "lock.h"
#include "bbq.h"
#include "cond.h"

/* bbq.c adapted from OSPP BBQ.cc */
/* thread-safe blocking queue */

/* Initialize the queue to empty,
   the lock to free, and the
   condition variables to empty.
*/
void
bbq_init(struct bbq *q)
{
    q->front = q->next_empty = 0;
    lock_init(&q->lk);
    cond_init(&q->item_removed_cv);
    cond_init(&q->item_added_cv);
}

/* Wait until there is room and
   then insert an item.
*/
void
bbq_insert(struct bbq *q, int item) {
    lock_acquire(&q->lk);

    while ((q->next_empty - q->front) == MAX) {
        cond_wait(&q->item_removed_cv, &q->lk);
    }
    q->items[q->next_empty % MAX] = item;
    q->next_empty++;

    cond_signal(&q->item_added_cv, &q->lk);
    
    lock_release(&q->lk);
}

/* Wait until there is an item and 
   then remove an item.
*/
int
bbq_remove(struct bbq *q)
{
    int item;

    lock_acquire(&q->lk);

    while (q->front == q->next_empty) {
	cond_wait(&q->item_added_cv, &q->lk);
    }

    item = q->items[q->front % MAX];
    q->front++;

    cond_signal(&q->item_removed_cv, &q->lk);
    lock_release(&q->lk);
    return item;
}
