#ifndef _LOCK_H_
#define _LOCK_H_
#include <stdbool.h>

#include "thread.h"

extern struct thread *current_thread;
struct lock {
    int busy;
    struct thread *holder;
    struct threadlist waitlist;
};

void lock_init (struct lock *lk);
void lock_acquire (struct lock *lk);
bool lock_try_acquire (struct lock *lk);
void lock_release (struct lock *lk);
bool lock_held_by_current_thread (const struct lock *lk);
#endif /* _LOCK_H_ */