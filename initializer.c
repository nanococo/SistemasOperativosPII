#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int lines = atoi(argv[1]);
        int size = sizeof(short) * lines;
        printf("\nYou have requested %d lines\n", lines);
        printf("Equivalent to %d bytes\n\n", size);

        // https://man7.org/linux/man-pages/man2/shmget.2.html
        // IPC_PRIVATE specifies that a new shared memory segment is wanted
        // IPC_CREAT flag stops shmget from looking for memory with the specified id and instead creates new shared memory
        // IPC_EXCL used with IPC_CREAT to ensure that shmget creates a new segment
        int shm_key = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666);
        printf("Shared memory ID: %d\n\n", shm_key);

        // https://linux.die.net/man/2/shmdt
        // attaches to shared memory
        short *mem = shmat(shm_key, NULL, 0);
        memset(mem, (short)0, size);

        /*
        Ask if log file needs to be shared or if it can be a regular file
        https://linux.die.net/man/2/open
        https://linux.die.net/man/2/close
        https://linux.die.net/man/2/unlink
        https://linuxhint.com/posix-shared-memory-c-programming/
        */

        // creates semaphore for shared memory access
        // https://man7.org/linux/man-pages/man3/sem_open.3.html
        sem_t *mem_mutex = sem_open("Memory Mutex", O_CREAT | O_EXCL, 0644, 1);

        // creates semaphore for log access
        sem_t *log_mutex = sem_open("Log Mutex", O_CREAT | O_EXCL, 0644, 1);

        sem_close(mem_mutex);
        sem_close(log_mutex);

        // Test code
        // From here to the end of this if statement
        // printf("Before procs.out call: mem[0] = %d\n\n", mem[0]);

        // sleep(10);

        // printf("After procs.out call: mem[0] = %d\n\n", mem[0]);

        // int deletion = shmctl(shm_key, IPC_RMID, NULL);

        // if (deletion < 0)
        // {
        //     printf("There was a problem deleting shared memory %d\n\n", shm_key);
        // }
    }
    else
    {
        printf("Missing line amount\nUsage:\n\t./init.out <line amount>\n\n");
    }
    return 0;
}