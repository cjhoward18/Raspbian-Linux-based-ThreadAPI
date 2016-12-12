#include <stdio.h>
#include "thread.h"

void thread1(void *arg) {
    printf("[thread1] first arg = %X\n", (unsigned) arg);
    thread_yield();
    printf("[thread1] second arg = %X\n", (unsigned) arg);
    thread_yield();
    thread_exit(0);
}

void thread2(void *arg) {
    printf("[thread2] first arg = %X\n", (unsigned) arg);
    thread_yield();
    printf("[thread2] second arg = %X\n", (unsigned) arg);
    thread_yield();
    thread_exit(0);
}

int main(int argc, char **argv)
{
    struct thread *t1, *t2;

    thread_init();
    printf("[main] After thread_init()\n");

    t1 = thread_create(thread1, (void *) 1);
    t2 = thread_create(thread2, (void *) 2);

    thread_yield();
    printf("[main] After thread_yield(t1)\n");
    thread_yield();
    printf("[main] After thread_yield(t2)\n");
    thread_yield();
    printf("[main] After thread_yield(t1)\n");
    thread_yield();
    printf("[main] After thread_yield(t2)\n");
    thread_join(t1);
    printf("[main] After thread_join(t1)\n");
    thread_join(t2);
    printf("[main] After thread_join(t2)\n");

    return 0;
}
