/*
 * CSC 308 - Operating Systems | Week 6
 * Practical Session 1: Mutex Lock Demonstration
 * Objective: Demonstrate mutual exclusion using pthread mutex locks
 *
 * Compile: gcc practical1.c -o practical1 -lpthread
 * Run:     ./practical1
 */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5
#define INCREMENT_COUNT 100000

int counter = 0;
pthread_mutex_t lock;

/* Thread function WITH mutex protection */
void *increment_with_mutex(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        pthread_mutex_lock(&lock);   // Acquire lock (Entry Section)
        counter++;                   // Critical Section
        pthread_mutex_unlock(&lock); // Release lock (Exit Section)
    }
    return NULL;
}

/* Thread function WITHOUT mutex protection (race condition demo) */
void *increment_without_mutex(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        counter++; // No protection - race condition!
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    /* ---- TEST 1: WITHOUT MUTEX (race condition) ---- */
    printf("=== TEST 1: WITHOUT MUTEX (Race Condition) ===\n");
    counter = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_without_mutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Expected counter value : %d\n", NUM_THREADS * INCREMENT_COUNT);
    printf("Actual counter value   : %d\n", counter);
    printf("Result: %s (data was lost due to race condition)\n\n",
           counter == NUM_THREADS * INCREMENT_COUNT ? "CORRECT" : "INCORRECT");

    /* ---- TEST 2: WITH MUTEX (correct) ---- */
    printf("=== TEST 2: WITH MUTEX (Mutual Exclusion) ===\n");
    counter = 0;
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Expected counter value : %d\n", NUM_THREADS * INCREMENT_COUNT);
    printf("Actual counter value   : %d\n", counter);
    printf("Result: %s\n",
           counter == NUM_THREADS * INCREMENT_COUNT ? "CORRECT" : "INCORRECT");

    pthread_mutex_destroy(&lock);
    return 0;
}
