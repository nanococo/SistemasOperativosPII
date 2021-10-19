#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int shm_id = atoi(argv[1]);

        sem_unlink ("Memory Mutex");
        sem_unlink ("Log Mutex");


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
    else
    {
        printf("Missing memory id\nUsage:\n\t./dest.out <memory id>\n\n");
    }
    return 0;
}