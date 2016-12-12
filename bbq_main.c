#include <assert.h>
#include <stdio.h>

#include "thread.h"
#include "bbq.h"
#include "cond.h"


void *put_some(void *p)
{
    int ii;
    struct bbq *queue = (struct bbq *) p;

    for(ii = 0; ii < 100; ii++){
	bbq_insert(queue, ii);
    }
    return NULL;
}  

int main(int argc, char **argv)
{
    
    struct bbq queues[3];
    struct thread *workers[3];
    int ii, jj, ret;

    thread_init();
    
    /* Start the worker threads */
    for(ii = 0; ii < 3; ii++){
	bbq_init(&queues[ii]);
	workers[ii] = thread_create( (void *) put_some, (void *) &queues[ii]);
    }
    
    /* Remove from the queues */
    for(ii = 0; ii < 3; ii++){
	printf("Queue %d:\n", ii);
	for(jj = 0; jj < 20; jj++){
            printf("hello");

	    ret = bbq_remove(&queues[ii]);
	    printf("Got %d\n", ret);
	}
    }
}
