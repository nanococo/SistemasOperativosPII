#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int id = atoi(argv[1]);

        // performs control operation on specified shared memory
        // IPC_RMID specifies that the shared memory should be destroyed
        // IMPORTANT: memory will not be destroyed until it has no attatched processes
        int deletion = shmctl(id, IPC_RMID, NULL);
        
        if (deletion < 0) 
        {
            printf("There was a problem deleting shared memory %d\n\n", id);
        }
        else
        {
            printf("Shared memory %d deleted successfully\n\n", id);
        }

    }
    else
    {
        printf("Missing memory id\nUsage:\n\t./dest.out <memory id>\n\n");
    }
    return 0;
}