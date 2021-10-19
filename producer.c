#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    // attach to the shared memory address
    int mem_id = atoi(argv[1]);
    short *mem = shmat(mem_id, NULL, 0);

    sem_t *mem_mutex = sem_open("Memory Mutex", 0);
    sem_t *log_mutex = sem_open("Log Mutex", 0);

    sem_wait(mem_mutex);

    // write in shared memory address
    mem[0] = 8; // dummy test

    sem_post(mem_mutex);

    // sem_close(mem_mutex);
    // sem_close(log_mutex);

    // detach from shared memory address
    int detach_status = shmdt(mem);
    if (detach_status < 0)
    {
        printf("Error detaching shared memory\n");
    }
    return 0;
}