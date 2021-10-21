#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

int main(int argc, char **argv)
{
    srand(time(0)); //This sets the random seed based on time
    
    //Thread generation window 30s-60s
    int lowerLifeLimit = 30;
    int upperLifeLimit = 60;

    //Algorithm to use for threads. Reads it from argv[3]. Must be numeric
    int algorithm = 0;
    if(sscanf(argv[3], "%i", &algorithm) != 1){
        printf("Error: Parameter is not numeric.\n");
    } else if (algorithm > 3 || algorithm < 1){
        printf("Error: Not a valid parameter for memory algorithms.\n");
    } else {
        
        while(1){
            int sleepTime = getRandomBetweenTwoNumbers(lowerLifeLimit, upperLifeLimit);
            int arguments[3] = {algorithm, atoi(argv[1]), atoi(argv[2])}; //this will be passed as the argument

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, findSpace, (void *)arguments);
            pthread_detach(thread_id); //I don't care about what this thread does, so I detatch it 
                                       //and when it dies it returns resources to the system
            sleep(sleepTime);
        }

    }




    
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

//This is the thread function. It finds space in the shared memory array. 
//It receives a parameter indicating the alogirthm that will be used 
// 1 = First Fit; 2 = Best Fit; 3 = Worst Fit
void *findSpace(void *array){
    //Algorithm retrieval
    int *threadParameters = (int *) array;

    //Lines size 1-10
    int lowerLinesLimit = 1;
    int upperLinesLimit = 10;

    //Time spent on memory 20s-60s
    int lowerTimeLimit = 20;
    int upperTimeLimit = 60;

    //Actual values
    int linesSize = getRandomBetweenTwoNumbers(lowerLinesLimit, upperLinesLimit);
    int sleepTime = getRandomBetweenTwoNumbers(lowerTimeLimit, upperTimeLimit);

    //CRITICAL SECTION START 
    //--------------------------------------------------------------------------------------------
    //Here is where each thread must select the memory area. Only one thread is allowed to use the selection algorithms.
    sem_t *mem_mutex = sem_open("Memory Mutex", 0);
    sem_wait(mem_mutex); // <-- Actual block

    int algorithm = threadParameters[0];
    int algorithmResult = 0;
    int startIndex = 0;

    switch(algorithm){
        case 1:
    int algorithmResult = 0;
            firstFit(linesSize, &algorithmResult, &startIndex, threadParameters[1], threadParameters[2]);
            break;
        case 2:
            bestFit(linesSize, &algorithmResult, &startIndex, threadParameters[1]);
            break;
        case 3:
            worstFit(linesSize, &algorithmResult, &startIndex, threadParameters[1]);
            break;
    }

    sem_post(mem_mutex); // <-- Semaphore release / unblock

    //CRITICAL SECTION END 
    //--------------------------------------------------------------------------------------------

    if(algorithmResult!=1){
        pthread_exit(pthread_self()); //If algoirhtm did not find space then kill thread. 
    } else {
        sleep(sleepTime); //If it was succesful then sleep for the indicated time.
    }

    return NULL; //Thread is detached so it will die and return resources
}

int getRandomBetweenTwoNumbers(int lower, int upper){
    return (rand() % (upper - lower + 1)) + lower;
}

void firstFit(int linesSize, int *algorithmResult, int *startIndex, int mem_id, int memLines){
    short *mem = shmat(mem_id, NULL, 0);

    int countEmpty = 0;
    int countStartIndex = 0;


    for(int i=0; i<memLines; i++){
        if(mem[i]==1 || i==memLines-1){
            if(countEmpty>=linesSize && countEmpty != 0){
                //This means a succesful block was found. Proceed to fill it in and break out of loop

                for(int j=countStartIndex; j<=countEmpty; j++){
                    mem[j]=1;
                }

                break;
            } else {
                //Restart count
                countStartIndex = 0;
                countEmpty = 0;
            }
        } else {
            //Count empty block
            if(countEmpty==0){
                countStartIndex = i;
            }
            countEmpty++;
        }
    }



    return 0;
}

void bestFit(int *linesSize, int *startIndex){
    return 0;
}

void worstFit(int *linesSize, int *startIndex){
    return 0;
}
