#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#define ARRAY_SIZE 40

void rm_shm(int shm_id);

int main(int argc, char **argv)
{
    if (argc == 7)
    {
        kill(atoi(argv[6]), SIGKILL);

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
        printf("Missing memory IDs, log name, or PID\nUsage:\n\t./dest.out <memory IDs>, *, <log name>, <PID>\n\n");
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
        printf("There was a problem deleting shared memory %d\n", shm_id);
    }
    else
    {
        printf("Shared memory %d deleted successfully\n", shm_id);
    }
}

// void kill_thread(pthread_t pid)
// {
//     if (pid)
//     {
//         printf("Kill thread");
//     }
// }

// void kill_threads(int exec_id, int blocked_id, int current_id)
// {
//     pthread_t *exec_mem = shmat(exec_id, NULL, 0);
//     pthread_t *blocked_mem = shmat(blocked_id, NULL, 0);
//     pthread_t *current_mem = shmat(current_id, NULL, 0);

//     kill_thread(current_mem[0]);
//     for (int i = 0; i < ARRAY_SIZE; i++)
//     {
//         kill_thread(exec_mem[i]);
//         kill_thread(blocked_mem[i]);
//     }
// }

