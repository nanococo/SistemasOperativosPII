#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void dealocateMemory(int linesSize, int startIndex, int mem_id){
    short *mem = shmat(mem_id, NULL, 0);

    //set all blocks used back to zero
    int index = startIndex;
    for(int i = 0; i<linesSize; i++){
        mem[index]=0;
        index++;
    }


    printf("Succesfully dealocated memory for thread id = %lu\n", pthread_self());

    int detach_status = shmdt(mem);
    if (detach_status < 0)
    {
        printf("Error detaching shared memory\n");
    }
}

void firstFit(int linesSize, int *algorithmResult, int *startIndex, int mem_id, int memLines){
    short *mem = shmat(mem_id, NULL, 0);

    int result = 0;
    int countEmpty = 0;
    int countStartIndex = 0;


    for(int i=0; i<memLines; i++){
        //printf("%hu \n", mem[i]);
        if(mem[i]==1 || i==memLines-1 || countEmpty>=linesSize){
            if(countEmpty>=linesSize && countEmpty != 0){
                //This means a succesful block was found. Proceed to fill it in and break out of loop

                for(int j=countStartIndex; j<=countEmpty; j++){
                    mem[j]=1;
                }

                printf("Succesfully alocated memory for thread id = %lu witn %d lines\n", pthread_self(), linesSize);

                *algorithmResult = 1;
                *startIndex = countStartIndex;

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

    int detach_status = shmdt(mem);
    if (detach_status < 0)
    {
        printf("Error detaching shared memory\n");
    }
}

void bestFit(int linesSize, int *algorithmResult, int *startIndex, int mem_id, int memLines){
    
}

void worstFit(int linesSize, int *algorithmResult, int *startIndex, int mem_id, int memLines){
    
}

int getRandomBetweenTwoNumbers(int lower, int upper){
    return (rand() % (upper - lower + 1)) + lower;
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

    //Actual random values
    int linesSize = getRandomBetweenTwoNumbers(lowerLinesLimit, upperLinesLimit);
    int sleepTime = getRandomBetweenTwoNumbers(lowerTimeLimit, upperTimeLimit);

    //CRITICAL SECTION START 
    //--------------------------------------------------------------------------------------------
    //Here is where each thread must select the memory area. Only one thread is allowed to use the selection algorithms.
    sem_t *mem_mutex = sem_open("Memory Mutex", 0);
    sem_wait(mem_mutex); // <-- Actual block

    int algorithm = threadParameters[0]; //get algorithm type received from CLI
    int algorithmResult = 0;
    int startIndex = 0;

    //Calls algorithm based on cli argument
    switch(algorithm){
        case 1:
            firstFit(linesSize, &algorithmResult, &startIndex, threadParameters[1], threadParameters[2]);
            break;
        case 2:
            bestFit(linesSize, &algorithmResult, &startIndex, threadParameters[1], threadParameters[2]);
            break;
        case 3:
            worstFit(linesSize, &algorithmResult, &startIndex, threadParameters[1], threadParameters[2]);
            break;
    }

    sem_post(mem_mutex); // <-- Semaphore release / unblock

    //CRITICAL SECTION END 
    //--------------------------------------------------------------------------------------------

    if(algorithmResult!=1){
        printf("Thread id = %lu did not find space. Needed %d\n", pthread_self(), linesSize);
        pthread_exit(0); //If algoirhtm did not find space then kill thread. 
    } else {
        printf("Thread id = %lu sleeping for %d\n", pthread_self(), sleepTime);
        sleep(sleepTime); //If it was succesful then sleep for the indicated time.
    }


    //CRITICAL SECTION 2 START 
    //--------------------------------------------------------------------------------------------
    //Here is where each thread will dealocate memory area. Only one thread is allowed to use the dealocation.
    sem_wait(mem_mutex); // <-- Actual block

    dealocateMemory(linesSize, startIndex, threadParameters[1]);

    sem_post(mem_mutex); // <-- Semaphore release / unblock

    //CRITICAL SECTION 2 END 
    //--------------------------------------------------------------------------------------------



    return NULL; //Thread is detached so it will die and return resources
}





//args must be mem_id, numberOfLines, algorithm
int main(int argc, char **argv)
{
    srand(time(0)); //This sets the random seed based on time
    
    //Thread generation window 30s-60s
    int lowerLifeLimit = 10;
    int upperLifeLimit = 15;

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
            printf("New Thread created with id %lu\n", pthread_self());
            printf("Creator main method sleeping for %d seconds\n", sleepTime);
            sleep(sleepTime);
        }

    }




    
    // // attach to the shared memory address
    // int mem_id = atoi(argv[1]);
    // short *mem = shmat(mem_id, NULL, 0);

    // sem_t *mem_mutex = sem_open("Memory Mutex", 0);
    // sem_t *log_mutex = sem_open("Log Mutex", 0);

    // sem_wait(mem_mutex);

    // // write in shared memory address
    // mem[0] = 8; // dummy test

    // sem_post(mem_mutex);

    // // sem_close(mem_mutex);
    // // sem_close(log_mutex);

    // // detach from shared memory address
    // int detach_status = shmdt(mem);
    // if (detach_status < 0)
    // {
    //     printf("Error detaching shared memory\n");
    // }
    return 0;
}
