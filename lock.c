#include "lock.h"
#include <stdlib.h>
#include <stdio.h>

/*Initializes lock*/
void lock_init(struct lock *lk) {
	list_init(&lk->waitlist);
	lk->holder = NULL;
	lk->busy = 0;
}

/*returns true if lock is free*/
bool lock_try_acquire(struct lock *lk) {
	if (lk->busy == 0) {
		return true;
	} else {
		return false;
	}
}

/*returns true if current thread is holding the lock*/
bool lock_held_by_current_thread(const struct lock *lk) {
	if (lk->holder == current_thread) {
		return true;
	} else {
		return false;
	}
}

/*tries to get lock, if not thread is added to the locks waitlist and blocks until woken up.*/
void lock_acquire(struct lock *lk) {
	if (lk->holder == current_thread) {
		printf("lock already held by thread. ");
		exit(-1);
	}
	if(lk->busy == 0) {
		lk->busy = 1;
		lk->holder = current_thread;
	} else {
		if (lk->waitlist.head == NULL) {
			lk->waitlist.head = current_thread;
			lk->waitlist.tail = current_thread;
		} else {
			lk->waitlist.tail->next = current_thread;
			lk->waitlist.tail = current_thread;
		}
		while(lk->holder != current_thread ) {
			current_thread->state == THREAD_WAITING;
			thread_yield();
		}
	}
}

/*releases lock and gives lock to next thread on the waitlist.*/
void lock_release(struct lock *lk) {
	if (lk->waitlist.head != NULL) {
		lk->holder = lk->waitlist.head;
		thread_ready(lk->waitlist.head);
		lk->waitlist.head = lk->waitlist.head->next;
	} else {
		lk->busy = 0;
		lk->holder = NULL;
	}
	thread_yield();
}
