#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static long long counter = 0;
static long long iterations = 100;

static void *worker(void *arg) {
    const char *name = (const char *)arg;

    for (long long i = 0; i < iterations; ++i) {
        __sync_fetch_and_add(&counter, 1);
    }

    printf("Inside thread %s.\n", name);
    printf("The counter: %lld\n", counter);
    return NULL;
}

static void run_case(int thread_count, long long iter_count) {
    pthread_t threads[3];
    const char *names[3] = {"A", "B", "C"};

    counter = 0;
    iterations = iter_count;

    for (int i = 0; i < thread_count; ++i) {
        pthread_create(&threads[i], NULL, worker, (void *)names[i]);
    }
    for (int i = 0; i < thread_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Inside main process.\n");
    printf("Final counter: %lld (expected: %lld)\n\n", counter, iter_count * thread_count);
}

int main(void) {
    printf("Case 1: two threads, 1E2 each\n");
    run_case(2, 100);

    printf("Case 2: two threads, 1E3 each\n");
    run_case(2, 1000);

    printf("Case 3: three threads, 1E2 each\n");
    run_case(3, 100);

    return 0;
}

/*
Explanation:
Fetch-and-add is atomic for every increment operation. Ordering of per-thread
print lines is non-deterministic, but final totals remain correct.
*/
