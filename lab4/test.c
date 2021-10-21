#include <stdio.h>
#include <string.h>

int main()
{
    char buff[256];
    char buff2[356];

    sprintf(buff, "%zu", sizeof(int));
    strcat(buff2, buff);
    sprintf(buff, "%s", "world");
    strcat(buff2, buff);

    printf("%s", buff2);
}