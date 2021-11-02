#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>

void printMemmory(short *mem, int memSize, sem_t *mutex);
void printProcessesStatus(int exec_shm_id, int blocked_shm_id, int current_shm_id);

int main(int argc, char **argv)
{
    if (argc == 5)
    {
        int mem_shm_id      = atoi(argv[1]);
        // int exec_shm_id     = argv[2];
        // int blocked_shm_id  = argv[3];
        // int current_shm_id  = argv[4];


        short *mem          = shmat(mem_shm_id, NULL, 0);

        sem_t *mem_mutex    = sem_open("Memory Mutex", 0);

        printMemmory(mem, 5, mem_mutex);

    }
    else
    {
        printf("Missing memory IDs\nUsage:\n\t./spy.out <memory IDs>, *\n\n");
    }
    return 0;
}

void printMemmory(short *mem, int memSize, sem_t *mutex){
    
    const char lineString[]  = "             ----- \n";
    printf("Memory: \n");
    

    sem_wait(mutex);
    for (int line = 0; line < memSize; line++)
    {
        printf("Line: %d -->  | %d | \n", line, mem[line]);
        printf(lineString);
    }
    sem_post(mutex);
    
    //printf(lineString);

}

void printProcessesStatus(int exec_shm_id, int blocked_shm_id, int current_shm_id){

}