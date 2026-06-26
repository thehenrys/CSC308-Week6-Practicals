/*
 * Practical Session 3: Semaphore Implementation in C
 * Objective: Compare mutex locks and semaphores for protecting shared resources
 *            Also experiment with counting semaphores (allow N threads at once)
 *
 * Compile: gcc practical3.c -o practical3 -lpthread -lrt
 * Run:     ./practical3
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS     6
#define INCREMENT_COUNT 50000
#define MAX_CONCURRENT  3  // Max threads allowed simultaneously (counting semaphore)

int counter = 0;

pthread_mutex_t mutex_lock;
sem_t binary_sem;
sem_t counting_sem;

/* ---- TEST 1: Protect counter with pthread_mutex ---- */
void *mutex_increment(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        pthread_mutex_lock(&mutex_lock);
        counter++;
        pthread_mutex_unlock(&mutex_lock);
    }
    return NULL;
}

/* ---- TEST 2: Protect counter with binary semaphore ---- */
void *semaphore_increment(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        sem_wait(&binary_sem);  // P() / down() / wait()
        counter++;
        sem_post(&binary_sem);  // V() / up() / signal()
    }
    return NULL;
}

/* ---- TEST 3: Counting semaphore - allows MAX_CONCURRENT threads in at once ---- */
void *counting_sem_task(void *arg) {
    int id = *(int *)arg;

    sem_wait(&counting_sem); // Acquire one resource slot

    printf("  Thread %d ENTERED  (max %d allowed at once)\n", id, MAX_CONCURRENT);
    sleep(1); // Simulate work inside the section
    printf("  Thread %d EXITING\n", id);

    sem_post(&counting_sem); // Release the resource slot
    return NULL;
}

/* Helper: measure elapsed time */
double elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0
         + (end.tv_nsec - start.tv_nsec) / 1e6;
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct timespec t_start, t_end;
    int ids[NUM_THREADS];

    printf("============================================\n");
    printf(" CSC308 Practical 3: Semaphore vs Mutex\n");
    printf("============================================\n\n");

    /* ======== TEST 1: MUTEX ======== */
    printf("--- TEST 1: pthread_mutex ---\n");
    counter = 0;
    pthread_mutex_init(&mutex_lock, NULL);

    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, mutex_increment, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    printf("Expected: %d | Got: %d | Time: %.2f ms\n",
           NUM_THREADS * INCREMENT_COUNT, counter, elapsed_ms(t_start, t_end));
    printf("Result: %s\n\n",
           counter == NUM_THREADS * INCREMENT_COUNT ? "CORRECT" : "INCORRECT");

    pthread_mutex_destroy(&mutex_lock);

    /* ======== TEST 2: BINARY SEMAPHORE ======== */
    printf("--- TEST 2: Binary Semaphore ---\n");
    counter = 0;
    sem_init(&binary_sem, 0, 1); // Initial value = 1 (unlocked)

    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, semaphore_increment, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    printf("Expected: %d | Got: %d | Time: %.2f ms\n",
           NUM_THREADS * INCREMENT_COUNT, counter, elapsed_ms(t_start, t_end));
    printf("Result: %s\n\n",
           counter == NUM_THREADS * INCREMENT_COUNT ? "CORRECT" : "INCORRECT");

    sem_destroy(&binary_sem);

    /* ======== TEST 3: COUNTING SEMAPHORE ======== */
    printf("--- TEST 3: Counting Semaphore (max %d threads at once) ---\n",
           MAX_CONCURRENT);
    sem_init(&counting_sem, 0, MAX_CONCURRENT);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, counting_sem_task, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    sem_destroy(&counting_sem);

    printf("\n============================================\n");
    printf(" Summary:\n");
    printf("  - Mutex:            binary, single resource, fast\n");
    printf("  - Binary Semaphore: binary, similar to mutex\n");
    printf("  - Counting Semaphore: allows up to N concurrent threads\n");
    printf("============================================\n");

    return 0;
}
