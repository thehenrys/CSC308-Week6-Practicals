
# CSC 308 - Operating Systems | Week 6
## Process Synchronization Practicals

### Practical 1: Mutex Lock Demonstration
File: practical1.c
Compile: gcc practical1.c -o practical1 -lpthread
Demonstrates race condition WITHOUT mutex, then correct result WITH mutex.

### Practical 2: Producer-Consumer Simulation
File: practical2.c
Compile: gcc practical2.c -o practical2 -lpthread -lrt
Bounded-buffer problem using POSIX semaphores (mutex=1, empty=N, full=0).

### Practical 3: Semaphore Implementation
File: practical3.c
Compile: gcc practical3.c -o practical3 -lpthread -lrt
Compares pthread_mutex vs binary semaphore, experiments with counting semaphore.

### Practical 4: Shared Memory Programming
File: practical4.c
Compile: gcc practical4.c -o practical4
IPC using shmget, shmat, shmdt, shmctl with semaphore synchronization.