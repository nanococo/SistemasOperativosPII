#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include "constants.h"

void create_shared_mem(int size);
void create_log();

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int lines = atoi(argv[1]);
        int size = sizeof(pthread_t) * lines;

        create_shared_mem(size);
        create_log();

        // creates semaphore for shared memory access
        // https://man7.org/linux/man-pages/man3/sem_open.3.html
        sem_t *mem_mutex = sem_open(MEMORY_MUTEX_NAME, O_CREAT | O_EXCL, 0644, 1);

        // creates semaphore for log access
        sem_t *log_mutex = sem_open(LOG_MUTEX_NAME, O_CREAT | O_EXCL, 0644, 1);

        // creates semaphore for spy info
        sem_t *spy_mutex = sem_open(SPY_MUTEX_NAME, O_CREAT | O_EXCL, 0644, 1);

        sem_close(mem_mutex);
        sem_close(log_mutex);
        sem_close(spy_mutex);
    }
    else
    {
        printf("Missing line amount\nUsage:\n\t./init.out <line amount>\n\n");
    }
    return 0;
}

void create_shared_mem(int size)
{
    // https://man7.org/linux/man-pages/man2/shmget.2.html
    // IPC_PRIVATE specifies that a new shared memory segment is wanted
    // IPC_CREAT flag stops shmget from looking for memory with the specified id and instead creates new shared memory
    // IPC_EXCL used with IPC_CREAT to ensure that shmget creates a new segment
    int shm_key = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666);

    // shared memory for program logic
    int pid_size = sizeof(pthread_t);

    int shm_exec_key = shmget(IPC_PRIVATE, pid_size*40, IPC_CREAT | IPC_EXCL | 0666);
    int shm_blocked_key = shmget(IPC_PRIVATE, pid_size*40, IPC_CREAT | IPC_EXCL | 0666);
    int shm_current_key = shmget(IPC_PRIVATE, pid_size, IPC_CREAT | IPC_EXCL | 0666);
    printf("Destroyer params: %d %d %d %d %s \n", shm_key, shm_exec_key, shm_blocked_key, shm_current_key, LOG_FILE_NAME);

    // https://linux.die.net/man/2/shmdt
    // attaches to shared memory
    pthread_t *mem = shmat(shm_key, NULL, 0);
    memset(mem, (pthread_t)0, size);

    pthread_t *exec_mem = shmat(shm_exec_key, NULL, 0);
    memset(exec_mem, (pthread_t) 0, pid_size*40);

    pthread_t *blocked_mem = shmat(shm_blocked_key, NULL, 0);
    memset(blocked_mem, (pthread_t)0, pid_size*40);

    pthread_t *current_mem = shmat(shm_current_key, NULL, 0);
    memset(current_mem, (pthread_t)0, pid_size);
}

void create_log()
{
    time_t t = time(NULL);
    // https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm
    struct tm time = *localtime(&t);
    FILE* file_ptr = fopen(LOG_FILE_NAME, "w");
    fputs("File created\n", file_ptr);
    fclose(file_ptr);
}