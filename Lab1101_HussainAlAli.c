#include <pthread.h>
#include <stdio.h>

#define LIMIT 10000000LL

static long long counter = 0;
static pthread_mutex_t mutex;

static void *worker(void *arg) {
    const char *name = (const char *)arg;

    pthread_mutex_lock(&mutex);
    for (long long i = 0; i < LIMIT; ++i) {
        counter++;
    }
    printf("Inside thread %s.\n", name);
    printf("The counter: %lld\n", counter);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void) {
    pthread_t thread_a, thread_b;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&thread_a, NULL, worker, "A");
    pthread_create(&thread_b, NULL, worker, "B");

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);

    pthread_mutex_destroy(&mutex);

    printf("Inside main process.\n");
    printf("The counter: %lld\n", counter);
    return 0;
}
