#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "constants.h"

void addToCurrentShm(pthread_t *current_shm);
void removeFromCurrentShm(pthread_t *current_shm);

void addToExecShm(pthread_t *exec_shm);
void removeFromExecShm(pthread_t *exec_shm);

void addToBlockedShm(pthread_t *blocked_shm);
void removeFromBlockedShm(pthread_t *blocked_shm);

void create_log(char message[]){
    sem_t *log_mutex = sem_open("Log Mutex", 0);
    sem_wait(log_mutex); // <-- Actual block

    time_t t = time(NULL);
    // https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm
    struct tm time = *localtime(&t);
    char logMessage[1024];

    sprintf((char *) &logMessage[0], "%d-%d-%d_%d:%d:%d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    // sprintf((char *) &file_name[23], ".txt");
    strcat(logMessage, ": ");
    strcat(logMessage, message);
    strcat(logMessage, "\n");

    printf(" %s\n", logMessage);


    FILE* file_ptr = fopen("log.txt", "a");
    fputs(logMessage, file_ptr);
    fclose(file_ptr);

    sem_post(log_mutex); // <-- Actual block
}

void dealocateMemory(int linesSize, int startIndex, int mem_id){
    short *mem = shmat(mem_id, NULL, 0);

    //set all blocks used back to zero
    int index = startIndex;
    for(int i = 0; i<linesSize; i++){
        mem[index]=0;
        index++;
    }

    //Buffer        
    char buffer[200];

    snprintf(buffer, sizeof(buffer), "Succesfully dealocated memory for thread id = %lu\n", pthread_self());
    printf("%s", buffer);
    create_log(buffer);

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
        if(mem[i]==1 || i==memLines-1){
            if(countEmpty>=linesSize && countEmpty != 0){
                //This means a succesful block was found. Proceed to fill it in and break out of loop
                int numberOfLinesLoop = linesSize;
                int loopIndex = countStartIndex;
                while(numberOfLinesLoop>0){
                    mem[loopIndex] = 1;
                    numberOfLinesLoop--;
                    loopIndex++;
                }

                //Buffer        
                char buffer[200];

                snprintf(buffer, sizeof(buffer), "Succesfully alocated memory for thread id = %lu with %d lines\n", pthread_self(), linesSize);
                printf("%s", buffer);
                create_log(buffer);

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
    short *mem = shmat(mem_id, NULL, 0);
    
    int maxSize = 0; // This will hold the size for the biggest block
    int maxBlockIndex = 0; // This will mark the beggining of the biggest block

    int countEmpty = 0;
    int countStartIndex = 0;


    for(int i=0; i<memLines; i++){
        //printf("%hu \n", mem[i]);

        if(mem[i]==1 || i==memLines-1){
            
            if(countEmpty >= maxSize && countEmpty>=linesSize){
                maxSize = countEmpty;
                maxBlockIndex = countStartIndex;
                countEmpty = 0;
            } else {
                countEmpty = 0;
            }

        } else {
            //Count empty block
            if(countEmpty==0){
                countStartIndex = i;
            }
            countEmpty++;
        }

        if(maxSize != 0 && maxSize>=linesSize){
            int numberOfLinesLoop = linesSize;
            int loopIndex = maxBlockIndex;
            while(numberOfLinesLoop>0){
                mem[loopIndex] = 1;
                numberOfLinesLoop--;
                loopIndex++;
            }

            //Buffer        
            char buffer[200];

            snprintf(buffer, sizeof(buffer), "Succesfully alocated memory for thread id = %lu with %d lines\n", pthread_self(), linesSize);
            printf("%s", buffer);
            create_log(buffer);

            *algorithmResult = 1;
            *startIndex = maxBlockIndex;
        } 
    }
}

void worstFit(int linesSize, int *algorithmResult, int *startIndex, int mem_id, int memLines){
    short *mem = shmat(mem_id, NULL, 0);
    
    int minSize = __INT_MAX__; // This will hold the size for the smallest block
    int minBlockIndex = 0; // This will mark the beggining of the smallest block

    int countEmpty = 0;
    int countStartIndex = 0;

    for(int i=0; i<memLines; i++){
        //printf("%hu \n", mem[i]);

        if(mem[i]==1 || i==memLines-1){
            
            if(countEmpty <= minSize && countEmpty>=linesSize){
                minSize = countEmpty;
                minBlockIndex = countStartIndex;
                countEmpty = 0;
            } else {
                countEmpty = 0;
            }

        } else {
            //Count empty block
            if(countEmpty==0){
                countStartIndex = i;
            }
            countEmpty++;
        }

        if(minSize != __INT_MAX__ && minSize>=linesSize){
            int numberOfLinesLoop = linesSize;
            int loopIndex = minBlockIndex;
            while(numberOfLinesLoop>0){
                mem[loopIndex] = 1;
                numberOfLinesLoop--;
                loopIndex++;
            }

            //Buffer        
            char buffer[200];

            snprintf(buffer, sizeof(buffer), "Succesfully alocated memory for thread id = %lu with %d lines\n", pthread_self(), linesSize);
            printf("%s", buffer);
            create_log(buffer);

            *algorithmResult = 1;
            *startIndex = minBlockIndex;
        } 
    }    
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

    //Shared memory ids
    int exec_shm_id = threadParameters[3];
    int blocked_shm_id = threadParameters[4];
    int current_shm_id = threadParameters[5];

    pthread_t *exec_shm = shmat(exec_shm_id, NULL, 0);
    pthread_t *blocked_shm = shmat(blocked_shm_id, NULL, 0);
    pthread_t *current_shm = shmat(current_shm_id, NULL, 0);

    //Lines size 1-10
    int lowerLinesLimit = 1;
    int upperLinesLimit = 10;

    //Time spent on memory 20s-60s
    int lowerTimeLimit = 20;
    int upperTimeLimit = 60;

    //Actual random values
    int linesSize = getRandomBetweenTwoNumbers(lowerLinesLimit, upperLinesLimit);
    int sleepTime = getRandomBetweenTwoNumbers(lowerTimeLimit, upperTimeLimit);

    //Buffer
    char buffer[200];

    //CRITICAL SECTION START 
    //--------------------------------------------------------------------------------------------
    //Here is where each thread must select the memory area. Only one thread is allowed to use the selection algorithms.
    snprintf(buffer, sizeof(buffer), "Thread id = %lu is waiting for semaphore\n", pthread_self());
    printf("%s", buffer);
    create_log(buffer);


    sem_t *mem_mutex = sem_open("Memory Mutex", 0);


    addToBlockedShm(blocked_shm);

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
        snprintf(buffer, sizeof(buffer), "Thread id = %lu did not find space. Needed %d\n", pthread_self(), linesSize);
        printf("%s", buffer);
        create_log(buffer);

        pthread_exit(0); //If algoirhtm did not find space then kill thread. 
    } else {
        printf("Thread id = %lu sleeping for %d\n", pthread_self(), sleepTime);
        sleep(sleepTime); //If it was succesful then sleep for the indicated time.
        removeFromExecShm(exec_shm);
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



//args must be numberOfLines, algorithm, mem_id, execId, blockedId, currentId
int main(int argc, char **argv)
{
    srand(time(0)); //This sets the random seed based on time
    
    //Thread generation window 30s-60s
    int lowerLifeLimit = 10;
    int upperLifeLimit = 15;

    int exec_shm_id = atoi(argv[4]);
    int blocked_shm_id = atoi(argv[5]);
    int current_shm_id = atoi(argv[6]);

    //Algorithm to use for threads. Reads it from argv[3]. Must be numeric
    int algorithm = 0;
    if(sscanf(argv[2], "%i", &algorithm) != 1){
        printf("Error: Parameter is not numeric.\n");
    } else if (algorithm > 3 || algorithm < 1){
        printf("Error: Not a valid parameter for memory algorithms.\n");
    } else {
        
        while(1){
            int sleepTime = getRandomBetweenTwoNumbers(lowerLifeLimit, upperLifeLimit);
            int arguments[6] = {algorithm, atoi(argv[3]), atoi(argv[1]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6])}; //this will be passed as the argument

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, findSpace, (void *)arguments);
            pthread_detach(thread_id); //I don't care about what this thread does, so I detatch it 
                                       //and when it dies it returns resources to the system

            char buffer[200];
            snprintf(buffer, sizeof(buffer), "New Thread created with id %lu\n", thread_id);
            printf("%s", buffer);
            create_log(buffer);

            snprintf(buffer, sizeof(buffer), "Creator main method sleeping for %d seconds\n", sleepTime);
            printf("%s", buffer);
            create_log(buffer);
        
            sleep(sleepTime);
        }

    }

    return 0;
}


void addToCurrentShm(pthread_t *current_shm){
    current_shm[0] = pthread_self();
}

void removeFromCurrentShm(pthread_t *current_shm){
    current_shm[0] = (pthread_t)0;
}

void addToExecShm(pthread_t *exec_shm){
    for (int i = 0; i < EXEC_SHM_SIZE; i++)
    {
        if (exec_shm[i] == 0lu)
        {
            exec_shm[i] = pthread_self();
        }
    }
}

void removeFromExecShm(pthread_t *exec_shm){
    pthread_t self = pthread_self();
    for (int i = 0; i < EXEC_SHM_SIZE; i++)
    {
        if (exec_shm[i] == self)
        {
            exec_shm[i] = 0lu;
        }
    }
}

void addToBlockedShm(pthread_t *blocked_shm){
    for (int i = 0; i < BLOCKED_SHM_SIZE; i++)
    {
        if (blocked_shm[i] == 0lu)
        {
            blocked_shm[i] = pthread_self();
        }
    }
}

void removeFromBlockedShm(pthread_t *blocked_shm){
    pthread_t self = pthread_self();
    for (int i = 0; i < BLOCKED_SHM_SIZE; i++)
    {
        if (blocked_shm[i] == self)
        {
            blocked_shm[i] = 0lu;
        }
    }
}