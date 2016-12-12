#include "cond.h"
#include <stdlib.h>

/*initializes condition variable.*/
void cond_init(struct cond *cv) {
	list_init(&cv->waitlist);

}

/*blocks thread untill condition is true*/
void cond_wait (struct cond *cv, struct lock *lk){
    lock_release(lk);

	if(cv->waitlist.head == NULL) {
        cv->waitlist.head = current_thread;
        cv->waitlist.tail = current_thread;
    } else {
        cv->waitlist.tail->next = current_thread;
        cv->waitlist.tail = current_thread;
    }
    current_thread->state = THREAD_WAITING;
    thread_yield();
    lock_acquire(lk);
}

/*wakes up a single thread on condition variables waitlist*/
void cond_signal (struct cond *cv, struct lock *lk){
	struct thread *tcb = NULL;
	if (cv->waitlist.head != NULL) {
		tcb = cv->waitlist.head;
        cv->waitlist.head = cv->waitlist.head->next;
        tcb->next = NULL;
        thread_ready(tcb);
	}

}

/*wakes up all threads on conditions variables waitlist.*/
void cond_broadcast (struct cond *cv, struct lock *lk){
	if (cv->waitlist.head != NULL) {
        struct thread *curr = cv->waitlist.head;
        
        while (curr != NULL) {
            cv->waitlist.head = cv->waitlist.head->next;
            thread_ready(curr);
            curr = cv->waitlist.head;
        }
        cv->waitlist.tail = NULL;
    }    
}
