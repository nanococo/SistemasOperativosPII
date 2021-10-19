#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int lines = atoi(argv[1]);
        printf("You have requested %d lines\n", lines);
    }
    else
    {
        printf("Missing line amount\nUsage:\n\t./init.out <line amount>\n\n");
    }
    return 0;
}