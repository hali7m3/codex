#include <pthread.h>
#include <sched.h>
#include <stdio.h>

#define LIMIT 10000LL

static volatile int lock_word = 0;
static long long counter = 0;

static inline void lock_cas(void) {
    while (__sync_val_compare_and_swap(&lock_word, 0, 1) != 0) {
        sched_yield();
    }
}

static inline void unlock_cas(void) { __sync_lock_release(&lock_word); }

static void *worker(void *arg) {
    const char *name = (const char *)arg;

    lock_cas();
    for (long long i = 0; i < LIMIT; ++i) {
        counter++;
    }
    printf("Inside thread %s.\n", name);
    printf("The counter: %lld\n", counter);
    unlock_cas();

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

Explanation:
CAS lock is atomic and ensures mutual exclusion (correct final count = 2*LIMIT),
but high iteration counts with spinning can look very slow or appear stuck.
*/
