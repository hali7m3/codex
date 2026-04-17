#include <pthread.h>
#include <sched.h>
#include <stdio.h>

#define LIMIT 10000LL

static volatile int flag = 0; /* textbook single-variable flag lock */
static long long counter = 0;

static inline void lock_flag(void) {
    while (flag == 1) {
        sched_yield();
    }
    flag = 1;
}

static inline void unlock_flag(void) { flag = 0; }

static void *worker(void *arg) {
    const char *name = (const char *)arg;

    lock_flag();
    for (long long i = 0; i < LIMIT; ++i) {
        counter++;
    }
    printf("Inside thread %s.\n", name);
    printf("The counter: %lld\n", counter);
    unlock_flag();

    return NULL;
}

int main(void) {
    pthread_t thread_a, thread_b;

    pthread_create(&thread_a, NULL, worker, "A");
    pthread_create(&thread_b, NULL, worker, "B");

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);

    printf("Inside main process.\n");
    printf("The counter: %lld\n", counter);
    return 0;
}

/*
How to test requested conditions:
- change LIMIT to 10000 (1E4), 1000000 (1E6), 100000000 (1E8), 10000000000 (1E10)

Observed behavior summary:
- 1E4 and 1E6 often complete and can print 20000 / 2000000.
- 1E8 and 1E10 can hang or show inconsistent output.

Explanation:
This lock is incorrect because checking `flag` and setting `flag = 1` are not
an atomic operation. Two threads can enter the critical section simultaneously,
or one/both can spin indefinitely.
*/
