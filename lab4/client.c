#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h> // for dirname()/basename()
#include <time.h>
#include <stdbool.h>

#define MAX 256
#define PORT 1234

char line[MAX], ans[MAX];
int n;

struct sockaddr_in saddr;
int sfd;

int tokenize(char *pathname, char *output[], char *token, int *num)
{
    char *s;
    *num = 0;
    s = strtok(pathname, token);

    while (s)
    {
        output[(*num)++] = s; // token string pointers
        s = strtok(0, token);
    }
    output[*num] = 0; // arg[n] = NULL pointer
}

int ls(char *CWD)
{
    DIR *dir;
    struct dirent *file1;
    struct stat stat1;

    char buf[512];
    dir = opendir(CWD);
    while ((file1 = readdir(dir)) != NULL)
    {
        sprintf(buf, "%s/%s", CWD, file1->d_name);
        stat(buf, &stat1);
        printf("%zu", stat1.st_size);
        printf(" %s\n", file1->d_name);
    }
    closedir(dir);
}

int cat(char *pathname)
{
    FILE *fp;

    fp = fopen(pathname, "r");
    char c;
    c = fgetc(fp);
    while (c != EOF)
    {
        printf("%c", c);
        c = fgetc(fp);
    }

    fclose(fp);

    // else
    //     printf("\nERROR, could not open file: %s", pathname);
}

int main(int argc, char *argv[], char *env[])
{
    int n;
    char how[64];
    int i;
    const int cSize = 9; //  0        1         2     3       4      5     6        7
    char *commands[] = {"lmkdir", "lrmdir", "lrm", "lcd", "lpwd", "lls", "lcat", "get", "put"};

    printf("1. create a socket\n");
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        printf("socket creation failed\n");
        exit(0);
    }

    printf("2. fill in server IP and port number\n");
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(PORT);

    printf("3. connect to server\n");
    if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }

    printf("********  processing loop  *********\n");
    while (1)
    {
        char *tokCommands[MAX] = {NULL};
        char pathname[MAX];
        char CWD[MAX];
        char lineCpy[MAX];
        int tokNumber;
        int commandIndex;
        bool found = false;
        bool getCheck = false;
        bool putCheck = false;

        printf("input a line : ");
        bzero(line, MAX);        // zero out line[ ]
        fgets(line, MAX, stdin); // get a line (end with \n) from stdin
        getcwd(CWD, sizeof(CWD));

        line[strlen(line) - 1] = 0; // kill \n at end

        strcpy(lineCpy, line);
        tokenize(lineCpy, tokCommands, " ", &tokNumber);

        // printf("%s%s", tokCommands[0], tokCommands[1]);
        // printf("%s\n", CWD);

        strcpy(pathname, CWD);

        for (int i = 0; i < cSize; i++)
        {
            if (!strcmp(commands[i], tokCommands[0]))
            {
                commandIndex = i;
                if (tokCommands[1])
                {
                    strcat(pathname, "/");
                    strcat(pathname, tokCommands[1]);
                }
                break;
            }
        }
        // printf("%s", pathname);

        switch (commandIndex)
        {
        case 0:
            mkdir(pathname, 0755);
            found = true;
            break;
        case 1:
            rmdir(pathname);
            found = true;
            break;
        case 2:
            unlink(pathname);
            found = true;
            break;
        case 3:
            chdir(pathname);
            found = true;
            break;
        case 4:
            printf("\n%s\n", pathname);
            found = true;
            break;
        case 5:
            ls(pathname);
            found = true;
            break;
        case 6:
            cat(pathname);
            found = true;
            break;
        case 7:
            getCheck = true;
            found = true;
            break;
        case 8:
            putCheck = true;
            found = true;
            break;

        default:
            break;
        }
        printf("found= %d", found);

        if (line[0] == 0) // exit if NULL line
            exit(0);

        if (found)
        {
            // Send ENTIRE line to server
            n = write(sfd, line, MAX);
            printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

            // Read a line from sock and show it
            n = read(sfd, ans, MAX);
            printf("client: read  n=%d bytes; echo=%s\n", n, ans);

            if (putCheck)
            {
                FILE *fp = fopen(pathname, "r");

                if (fp != NULL)
                {
                    fseek(fp, 0L, SEEK_END);

                    long int length = ftell(fp);
                    char lengthAsString[MAX];
                    sprintf(lengthAsString, "%ld", length);

                    write(sfd, lengthAsString, MAX);
                    printf("tried to write %sbytes:\n", lengthAsString);

                    fclose(fp);
                    read(sfd, line, MAX);

                    fp = fopen(pathname, "r");
                    char file1[length];
                    fread(file1, sizeof(char), length, fp);
                    file1[length] = 0;
                    printf("%s\n", file1);
                    write(sfd, file1, length);
                }
                else
                {
                    printf("error opening file\n");
                }
            }
        }
    }
}
