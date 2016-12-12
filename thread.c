#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

/* Private functions */

void scheduler(void);
void ctx_switch(void **old, void *new);

/* Type abbreviations */
typedef unsigned int uint;

#define STACK_SIZE 4096
#define NTHREADS 16

/* Memory for thread stacks */
unsigned char memory[STACK_SIZE * NTHREADS];

/* Convenience pointer to stacks region */
void *stacks;

/* ARM v7 context layout */
struct context {
    uint    r4;
    uint    r5;
    uint    r6;
    uint    r7;
    uint    r8;
    uint    r9;
    uint    r10;
    uint    r11;
    uint    r12;
    uint    lr;
};

/* Pointers to important threads */
struct thread *current_thread = NULL;
struct thread *main_thread = NULL;
struct thread *scheduler_thread  = NULL;
unsigned char scheduler_stack[STACK_SIZE];
struct threadlist freelist;
struct threadlist readylist;

/* Initialize a tcb. Setup top of stack pointer. */
void thread_init_tcb(struct thread *tcb)
{
    tcb->state = THREAD_FREE;
    tcb->top = ((void *) tcb) + STACK_SIZE;
    tcb->sp = NULL;
    tcb->start_func = NULL;
    tcb->arg = NULL;
    tcb->exit_value = 0;
    list_init(&tcb->waitlist);
    tcb->next = NULL;
}

void list_init(struct threadlist *list) {
    list->head = NULL;
    list->tail = NULL;
}

/* Setup the register context in the stack
   Most important is the lr value, which is set to func so that we
   switch to func on a context switch.
*/
void thread_set_context(struct context *new_context, uint func)
{
    new_context->r4 = 0;
    new_context->r5 = 0;
    new_context->r6 = 0;
    new_context->r7 = 0;
    new_context->r8 = 0;
    new_context->r9 = 0;
    new_context->r10 = 0;
    new_context->r11 = 0;
    new_context->r12 = 0;
    new_context->lr = func;
}

/* Initialize the thread system.
   This must be called before any thread functions are used.
*/
void thread_init()
{
    int i;
    struct thread *tcb;
    /* Initialize global ready and free lists, */
    list_init(&freelist);
    list_init(&readylist);

    /* Point stacks to memory regions for allocating threads. */
    stacks = memory;
    /* Initialize all threads. */
    for (i = 0; i < NTHREADS; i++) {
	tcb = (struct thread *) (stacks + (i * STACK_SIZE));
	thread_init_tcb(tcb);
    if(freelist.head == NULL) {
        freelist.head = tcb;
        freelist.tail = tcb;
        } else {
            freelist.tail->next = tcb;
            freelist.tail = tcb;
        }
    }

    /* Associate a TCB with the current main thread. */
    main_thread = freelist.head;
    freelist.head = freelist.head->next;
    thread_init_tcb(main_thread);
    main_thread->state = THREAD_RUNNING;

    /* Set current thread so that we can switch from the main thread to other threads. */
    current_thread = main_thread;

    /* Create a scheduler thread. */
    scheduler_thread = (struct thread *) scheduler_stack;
    thread_init_tcb(scheduler_thread);
    scheduler_thread->sp = scheduler_thread->top - sizeof(struct context);
    thread_set_context((struct context *) scheduler_thread->sp, (uint) scheduler);
}

/* Get a thread from the thread array with the given state
   Return NULL if no thread found.
 */
struct thread *thread_get(int state)
{
    int i;
    struct thread *tcb = NULL;

    if (state == THREAD_FREE) {
        tcb = freelist.head;
        freelist.head = freelist.head->next;
        tcb->next = NULL;
    } else {
        if (state == THREAD_READY) {
            tcb = readylist.head;
            readylist.head = readylist.head->next;
            tcb->next = NULL;
        }
    
    return tcb;
    }
}

/* Put the given thread into the ready state. */
void thread_ready(struct thread *tcb)
{
    /* Moving a thread from its current list to the end of the readylist. */
    tcb->next = NULL;
    tcb->state = THREAD_READY;
    if(readylist.head == NULL) {
        readylist.head = tcb;
        readylist.tail = tcb;
        } else {
            readylist.tail->next = tcb;
            readylist.tail = tcb;
        }
    }
    

/* Block the given thread. */
void thread_block(struct thread *tcb)
{
    tcb->state = THREAD_WAITING;
}

/* Free the given thread and make it avaible to be used again. */
void thread_free(struct thread *tcb)
{   
    /* Adding to the end of the freelist. */
    tcb->next = NULL;
    tcb->state = THREAD_FREE;
    freelist.tail->next = tcb;
    freelist.tail = tcb;
}

/* This is the scheduler thread function. 
   It runs until there are no more ready threads.
*/
void scheduler(void)
{
    struct thread *next;

    while(1) {
	/* If the previous thread is in the RUNNING state, then set to READY. */
	if (current_thread->state == THREAD_RUNNING) {
	    thread_ready(current_thread);
	}
	/* Find the next thread to run. */
	next = thread_get(THREAD_READY);
	if (next == NULL) {
	    printf("No ready threads.\n");
	    exit(-1);
	}
	/* Setup current thread and switch to it. */
	current_thread = next;
	current_thread->state = THREAD_RUNNING;
	ctxswitch(&scheduler_thread->sp, next->sp);
    }
}

/* Helper function to start new threads.
   This is used to make sure all threads eventually call thread_exit()
   if the start_func returns before calling thread_exit().
*/
void thread_start(void)
{
    current_thread->start_func(current_thread->arg);
    thread_exit(-1);
}

/* Create a new thread with given start_func and arg. */
struct thread *thread_create(void (*start_func)(void *), void *arg)
{
    struct thread *new;
    struct context *new_context;

    new = thread_get(THREAD_FREE);
    new->sp = new->top - sizeof(struct context);
    new->arg = arg;
    new->start_func = start_func;
    new->state = THREAD_INIT;
    new->exit_value = 0;
    list_init(&new->waitlist);

    thread_set_context((struct context *) new->sp, (uint) thread_start);
    thread_ready(new);

    return new;
}

/* Yield the current thread and allow another thread to run. */
void thread_yield(void) {
    /* Always switch to scheduler thread. */
    ctxswitch(&current_thread->sp, scheduler_thread->sp);
}

/* Exit the current thread, possibly unblocking a thread waiting to join. */
void thread_exit(int value) {
    current_thread->exit_value = value;
    /* Check if the waitlist is empty. */
    if (current_thread->waitlist.head != NULL) {
        struct thread *curr = current_thread->waitlist.head;
        /* Freeing all threads waiting on current_thread exiting. */
        while (curr != NULL) {
            current_thread->waitlist.head = current_thread->waitlist.head->next;
            thread_ready(curr);
            curr = current_thread->waitlist.head;
        }
        current_thread->waitlist.tail = NULL;
    }    
    current_thread->state = THREAD_EXITING;
    thread_yield();
}

/* Join with another thread. Block and wait for the thread if it has not exited. */
int thread_join(struct thread *exit_thread)
{
    int value;
    /* Block this thread if exit_thread is still running. */
    if (exit_thread->state != THREAD_EXITING) {
        /* Adding to the end of the still running threads waitlist. */
        if(exit_thread->waitlist.head == NULL) {
            exit_thread->waitlist.head = current_thread;
            exit_thread->waitlist.tail = current_thread;
        } else {
            exit_thread->waitlist.tail->next = current_thread;
            exit_thread->waitlist.tail = current_thread;
            }

	   thread_block(current_thread);
	   thread_yield();
    }
    thread_free(exit_thread);
    value = exit_thread->exit_value;
    
    return value;
}
