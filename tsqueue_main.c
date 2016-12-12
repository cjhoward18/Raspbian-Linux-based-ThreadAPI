/* tsqueue_main.c based on OSPP TSQueueMain.cc
   Test code for tsqueue.
*/

#include <assert.h>
#include <stdio.h>
#include "thread.h"
#include "tsqueue.h"

void *put_some(void *p);
void test_removal(struct tsqueue *q);

int 
main(int argc, char **argv) {
    thread_init();
    struct tsqueue queues[3];
    struct thread *workers[3];
    int i, j;

    /* Start worker threads to insert. */
    for (i = 0; i < 3; i++) {
        tsqueue_init(&queues[i]);
        workers[i] = thread_create((void * ) put_some, (void *) &queues[i]);
    }

    /* Wait for some items to be put. */
    thread_join(workers[0]);

    /* Remove 20 items from each queue. */
    for (i = 0; i < 3; i++) {
        printf("Queue %d:\n", i);
        test_removal(&queues[i]);
    }
}

/* Insert 50 items into a queue. */
void *put_some(void *p) {
    struct tsqueue *queue = (struct tsqueue *) p;
    int i;

    for (i = 0; i < 50; i++) {
        tsqueue_try_insert(queue, i);
    }
    return NULL;
}  

/* Remove 20 items from a queue. */
void test_removal(struct tsqueue *queue) {
    int i, item;

    for (i = 0; i < 20; i++) {
        if (tsqueue_try_remove(queue, &item))
          printf("Removed %d\n", item);
        else
          printf("Nothing there.\n");
        }
    }
