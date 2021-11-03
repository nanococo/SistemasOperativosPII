#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>

#include "constants.h"

void printMemmory(pthread_t *mem, int memSize, sem_t *mutex);
void printProcessesStatus(pthread_t *exec_shm, pthread_t *blocked_shm, pthread_t *current_shm, sem_t *mutex);

int main(int argc, char **argv)
{
    if (argc == 5)
    {
        int number_of_lines = atoi(argv[1]);
        int mem_shm_id = atoi(argv[2]);
        int exec_shm_id = atoi(argv[3]);
        int blocked_shm_id = atoi(argv[4]);
        int current_shm_id = atoi(argv[5]);

        pthread_t *mem_shm = shmat(mem_shm_id, NULL, 0);
        pthread_t *exec_shm = shmat(exec_shm_id, NULL, 0);
        pthread_t *blocked_shm = shmat(blocked_shm_id, NULL, 0);
        pthread_t *current_shm = shmat(current_shm_id, NULL, 0);

        sem_t *mem_mutex = sem_open(MEMORY_MUTEX_NAME, 0);
        sem_t *spy_mutex = sem_open(SPY_MUTEX_NAME, 0);

        // ? Deberia evitar que alguno entre a la memoria para imprimir el actual?
        printMemmory(mem_shm, number_of_lines, mem_mutex);
        printProcessesStatus(exec_shm,blocked_shm,current_shm,spy_mutex);
    }
    else
    {
        printf("Missing memory IDs\nUsage:\n\t./spy.out <memory IDs>, *\n\n");
    }
    return 0;
}

void printMemmory(pthread_t *mem, int memSize, sem_t *mutex)
{
    const char lineString[] = "             ----- \n";

    printf("Memory: \n");

    sem_wait(mutex);
    for (int line = 0; line < memSize; line++)
    {
        printf("Line: %d -->  | %lu | \n", line, mem[line]);
        printf(lineString);
    }
    sem_post(mutex);
}

void printProcessesStatus(pthread_t *exec_shm, pthread_t *blocked_shm, pthread_t *current_shm, sem_t *mutex)
{
    printf("Current Process in Memory: \n");

    printf("%lu \n", current_shm[0]);

    printf("Blocked Processes: \n");

    for (int i = 0; i < BLOCKED_SHM_SIZE; i++)
    {
        pthread_t pid = blocked_shm[i];
        if (pid != 0)
        {
            printf("%lu \n", pid);
        }
    }

    printf("Executing Processes: \n");

    for (int i = 0; i < EXEC_SHM_SIZE; i++)
    {
        pthread_t pid = exec_shm[i];
        if (pid != 0)
        {
            printf("%lu \n", pid);
        }
    }
    
}