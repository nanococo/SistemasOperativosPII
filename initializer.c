#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int lines = atoi(argv[1]);
        int size = sizeof(short) * lines;
        printf("You have requested %d lines\n", lines);
        printf("Equivalent to %d bytes\n\n", size);

        // https://man7.org/linux/man-pages/man2/shmget.2.html
        // IPC_PRIVATE specifies that a new shared memory segment is wanted
        // IPC_CREAT flag stops shmget from looking for memory with the specified id and instead creates new shared memory
        // IPC_EXCL used with IPC_CREAT to ensure that shmget creates a new segment
        int shm_key = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666);
        printf("SHared memory ID: %d\n\n", shm_key);
    }
    else
    {
        printf("Missing line amount\nUsage:\n\t./init.out <line amount>\n\n");
    }
    return 0;
}