#include <pthread.h>
#include <sched.h>
#include <stdio.h>

#define LIMIT 1000LL

static volatile int lock_word = 0;
static long long counter = 0;

static inline int test_and_set(volatile int *ptr) {
    return __sync_lock_test_and_set(ptr, 1);
}

static inline void lock_tas(void) {
    while (test_and_set(&lock_word) == 1) {
        sched_yield();
    }
}

static inline void unlock_tas(void) { __sync_lock_release(&lock_word); }

static void *worker(void *arg) {
    const char *name = (const char *)arg;

    lock_tas();
    for (long long i = 0; i < LIMIT; ++i) {
        counter++;
    }
    printf("Inside thread %s.\n", name);
    printf("The counter: %lld\n", counter);
    unlock_tas();

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
- set LIMIT = 1000 (1E3), then LIMIT = 10000 (1E4)

Explanation:
Test-and-set is atomic and correctly enforces mutual exclusion, so the final
counter should be deterministic (2 * LIMIT). Performance may degrade as LIMIT
increases because this is a spin lock.
*/
