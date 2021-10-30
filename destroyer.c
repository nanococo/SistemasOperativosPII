#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

void rm_shm(int shm_id);

int main(int argc, char **argv)
{
    if (argc == 6)
    {
        // --------------------
        // --- Kill threads ---
        // --------------------

        // Delete shared memory
        for (int i = 1; i < 5; i++)
        {
            rm_shm(atoi(argv[i]));
        }

        // Close log file
        char* file_name = argv[5];
        FILE* file_ptr = fopen(file_name, "a");
        fclose(file_ptr);

        // Unlink semaphores
        sem_unlink ("Memory Mutex");
        sem_unlink ("Log Mutex");
        sem_unlink ("Spy Mutex");   
    }
    else
    {
        printf("Missing memory IDs or log name\nUsage:\n\t./dest.out <memory IDs>, *, <log name>\n\n");
    }
    return 0;
}

void rm_shm(int shm_id)
{
    // https://linux.die.net/man/2/shmctl
    // performs control operation on specified shared memory
    // IPC_RMID specifies that the shared memory should be destroyed
    // IMPORTANT: memory will not be destroyed until it has no attached processes
    int deletion = shmctl(shm_id, IPC_RMID, NULL);

    if (deletion < 0)
    {
        printf("There was a problem deleting shared memory %d\n\n", shm_id);
    }
    else
    {
        printf("Shared memory %d deleted successfully\n\n", shm_id);
    }
}