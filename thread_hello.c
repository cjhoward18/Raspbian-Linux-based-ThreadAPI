#include <stdio.h>
#include "thread.h"

static void go(void *arg);

#define NTHREADS 10
static struct thread *threads[NTHREADS];

int main(int argc, char **argv)
{
    int i;
    int exit_value;

    thread_init();

    for (i = 0; i < NTHREADS; i++) {
	threads[i] = thread_create(go, (void *) i);
    }

    for (i = 0; i < NTHREADS; i++) {
	exit_value = thread_join(threads[i]);
	printf("Thread %d returned with %d\n", i, exit_value);
    }
    printf("Main thread done.\n");
}

void go(void *arg) {
    int n = (int) arg;
    printf("Hello from thread %d\n", n);
    thread_exit(100 + n);
}
