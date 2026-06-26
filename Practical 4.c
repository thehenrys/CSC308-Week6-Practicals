/*
 * Practical Session 4: Shared Memory Programming
 * Objective: Implement inter-process communication using shared memory
 *
 * Compile: gcc practical4.c -o practical4
 * Run:     ./practical4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define SHM_SIZE 1024
#define SEM_NAME "/csc308_shm_sem"

int main() {
    printf("============================================\n");
    printf(" CSC308 Practical 4: Shared Memory IPC\n");
    printf("============================================\n\n");

    /* Step 1: Create shared memory segment */
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    printf("[PARENT] Shared memory segment created. ID: %d\n", shmid);

    /* Step 2: Create a named semaphore for synchronization */
    sem_unlink(SEM_NAME); // Remove any leftover semaphore
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        shmctl(shmid, IPC_RMID, NULL);
        exit(1);
    }

    /* Step 3: Fork to create child process */
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    /* ======== CHILD PROCESS: reads from shared memory ======== */
    if (pid == 0) {

        /* Attach shared memory in child */
        char *shm_ptr = (char *)shmat(shmid, NULL, 0);
        if (shm_ptr == (char *)-1) {
            perror("Child: shmat failed");
            exit(1);
        }

        printf("[CHILD]  Waiting for parent to write...\n");

        /* Wait until parent signals data is ready */
        sem_wait(sem);

        /* Step 3 (child): Read data from shared memory */
        printf("[CHILD]  Read from shared memory: \"%s\"\n", shm_ptr);

        /* Step 4 (child): Detach shared memory */
        shmdt(shm_ptr);

        /* Signal parent that reading is done */
        sem_post(sem);

        sem_close(sem);
        exit(0);
    }

    /* ======== PARENT PROCESS: writes to shared memory ======== */
    else {

        /* Attach shared memory in parent */
        char *shm_ptr = (char *)shmat(shmid, NULL, 0);
        if (shm_ptr == (char *)-1) {
            perror("Parent: shmat failed");
            exit(1);
        }

        sleep(1); // Give child time to start and wait

        /* Step 3 (parent): Write data to shared memory */
        const char *message = "Hello from CSC308 Practical 4!";
        strncpy(shm_ptr, message, SHM_SIZE - 1);
        printf("[PARENT] Written to shared memory: \"%s\"\n", message);

        /* Signal child that data is ready */
        sem_post(sem);

        /* Wait for child to finish reading */
        sem_wait(sem);
        printf("[PARENT] Child has read the data.\n");

        /* Step 4 (parent): Detach shared memory */
        shmdt(shm_ptr);

        /* Wait for child process to exit */
        wait(NULL);

        /* Step 5: Cleanup - remove shared memory segment */
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID failed");
        } else {
            printf("[PARENT] Shared memory segment removed.\n");
        }

        /* Remove named semaphore */
        sem_close(sem);
        sem_unlink(SEM_NAME);

        printf("\n[DONE] IPC via shared memory completed successfully.\n");
        printf("============================================\n");
        printf(" Summary of steps performed:\n");
        printf("  1. shmget()        - Created shared memory\n");
        printf("  2. fork()          - Created child process\n");
        printf("  3. shmat()         - Attached memory in both processes\n");
        printf("  4. sem_open/wait/post - Synchronized access\n");
        printf("  5. shmdt()         - Detached memory\n");
        printf("  6. shmctl(IPC_RMID)- Removed memory segment\n");
        printf("============================================\n");
    }

    return 0;
}
