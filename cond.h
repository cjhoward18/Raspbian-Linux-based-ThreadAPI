#ifndef _COND_H_
#define _COND_H_
#include "thread.h"
#include "lock.h"
#include <stdbool.h>


struct cond {
	struct threadlist waitlist;

};

void cond_init (struct cond *cv);
void cond_wait (struct cond *cv, struct lock *lk);
void cond_signal (struct cond *cv, struct lock *lk);
void cond_broadcast (struct cond *cv, struct lock *lk);

#endif /* _COND_H_ */
