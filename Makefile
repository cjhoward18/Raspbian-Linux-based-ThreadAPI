DEBUG=-g
FILES = thread_test thread_hello thread.o ctxswitch.o tsqueue_main lock bbq_main

all: thread_test thread_hello tsqueue_main bbq_main

thread_test: thread_test.c thread.o ctxswitch.o
	gcc ${DEBUG} -o thread_test thread_test.c thread.o ctxswitch.o

thread_hello: thread_hello.c thread.o ctxswitch.o
	gcc ${DEBUG} -o thread_hello thread_hello.c thread.o ctxswitch.o

tsqueue_main: tsqueue_main.c tsqueue.o thread.o ctxswitch.o lock.o 
	gcc ${DEBUG} -o tsqueue_main tsqueue_main.c tsqueue.o thread.o ctxswitch.o lock.o

bbq_main:	bbq_main.c bbq.o thread.o ctxswitch.o lock.o cond.o
	gcc ${DEBUG} -o bbq_main bbq_main.c bbq.o thread.o ctxswitch.o lock.o cond.o

bbq.o: bbq.c bbq.h
	gcc -c -o bbq.o bbq.c
	
tsqueue.o: tsqueue.c tsqueue.h
	gcc ${DEBUG} -c -o tsqueue.o tsqueue.c 

thread.o: thread.c thread.h
	gcc ${DEBUG} -c -o thread.o thread.c

ctxswitch.o: ctxswitch.s
	gcc -c -o ctxswitch.o ctxswitch.s

lock.o: lock.c lock.h
	gcc -c -o lock.o lock.c

cond.o: cond.c cond.h
	gcc -c -o cond.o cond.c 


clean:
	rm -f ${FILES}

