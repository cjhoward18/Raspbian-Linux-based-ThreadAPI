#ifndef _THREAD_H_
#define _THREAD_H_

/* public data types */

/* thread states */
#define THREAD_FREE     0
#define THREAD_INIT     1
#define THREAD_READY    2
#define THREAD_RUNNING  3
#define THREAD_WAITING  4
#define THREAD_EXITING  5

struct threadlist {
    struct thread *head;
    struct thread *tail;
};

void list_init(struct threadlist *list);

struct thread {
    void *top; // top of the stack for this thread
    void *sp;  // current stack pointer for this thread (context)
    void (*start_func)(void *);
    void *arg;
    int state;
    int exit_value;
    struct threadlist waitlist;
    struct thread *next;
};



/* public functions */

void thread_init();
struct thread *thread_create(void (*start_func)(void *), void *arg);
void thread_yield(void);
void thread_exit(int value);
int thread_join(struct thread *t);

#endif /* _THREAD_H_ */
