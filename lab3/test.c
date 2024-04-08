#include <stdio.h>
#include <stdlib.h>

int main()
{
    srand(time(NULL));
    //printf("%d\n", rand()%2);
    printf("%d\n", getrand());

    return 0;
}