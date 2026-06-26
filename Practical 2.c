/*
 * CSC 308 - Operating Systems | Week 6
 * Practical Session 2: Producer-Consumer Simulation
 * Objective: Implement Producer-Consumer problem using POSIX semaphores
 *
 * Compile: gcc practical2.c -o practical2 -lpthread -lrt
 * Run:     ./practical2
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS   10

/* Circular buffer */
int buffer[BUFFER_SIZE];
int in  = 0; // Producer inserts here
int out = 0; // Consumer removes from here

/* Semaphores */
sem_t mutex; // Mutual exclusion (1)
sem_t empty; // Count of empty slots (N)
sem_t full;  // Count of filled slots (0)

/* ---- PRODUCER THREAD ---- */
void *producer(void *arg) {
    for (int item = 1; item <= NUM_ITEMS; item++) {

        sem_wait(&empty);          // Wait for an empty slot
        sem_wait(&mutex);          // Enter critical section

        /* --- Critical Section --- */
        buffer[in] = item;
        printf("[PRODUCER] Produced item %2d --> buffer[%d]\n", item, in);
        in = (in + 1) % BUFFER_SIZE;
        /* --- End Critical Section --- */

        sem_post(&mutex);          // Exit critical section
        sem_post(&full);           // Signal that buffer has an item

        sleep(1); // Simulate production time
    }
    return NULL;
}

/* ---- CONSUMER THREAD ---- */
void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {

        sem_wait(&full);           // Wait for a filled slot
        sem_wait(&mutex);          // Enter critical section

        /* --- Critical Section --- */
        int item = buffer[out];
        printf("[CONSUMER] Consumed item %2d <-- buffer[%d]\n", item, out);
        out = (out + 1) % BUFFER_SIZE;
        /* --- End Critical Section --- */

        sem_post(&mutex);          // Exit critical section
        sem_post(&empty);          // Signal that a slot is now empty

        sleep(2); // Consumer is slower than producer
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;

    printf("=== Producer-Consumer Problem (Bounded Buffer) ===\n");
    printf("Buffer Size: %d | Items to produce/consume: %d\n\n",
           BUFFER_SIZE, NUM_ITEMS);

    /* Initialize semaphores */
    sem_init(&mutex, 0, 1);           // mutex = 1
    sem_init(&empty, 0, BUFFER_SIZE); // empty = N (all slots free)
    sem_init(&full,  0, 0);           // full  = 0 (no items yet)

    /* Create threads */
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    /* Wait for threads to finish */
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    /* Cleanup */
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("\n=== Simulation Complete ===\n");
    return 0;
}
