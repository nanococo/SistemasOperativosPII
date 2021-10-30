#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

void create_log();

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

        create_log();


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

void create_log()
{
    time_t t = time(NULL);
    // https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm
    struct tm time = *localtime(&t);
    char file_name[30] = "log_";

    sprintf((char *) &file_name[4], "%d-%d-%d_%d:%d:%d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    sprintf((char *) &file_name[23], ".txt");

    FILE* file_ptr = fopen(file_name, "w");
    fputs("File created", file_ptr);
    fclose(file_ptr);
}