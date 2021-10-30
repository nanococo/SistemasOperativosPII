#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("PID: %d\n", getpid());
    while (1)
    {
        sleep(5);
    }
    return 0;
}