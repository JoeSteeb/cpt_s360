/***** LAB3 base code *****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

char gpath[128]; // hold token strings
char *arg[64];   // token string pointers
int n;           // number of token strings

char dpath[128]; // hold dir strings in PATH
char *dir[64];   // dir string pointers
int ndir;        // number of dirs

int tokenize(char *pathname, char *output[], char *token, char *holder, int *num) // YOU have done this in LAB2
{                                                                                 // YOU better know how to apply it from now on
    char *s;
    strcpy(holder, pathname); // copy into global gpath[]
    s = strtok(holder, token);
    *num = 0;

    while (s)
    {
        output[(*num)++] = s; // token string pointers
        s = strtok(0, token);
    }
    output[*num] = 0; // arg[n] = NULL pointer
}

int main(int argc, char *argv[], char *env[])
{
    int i;
    int pid, status;
    char *cmd;
    char line[28];
    char *space = " ";
    char *colon = ":";

    for (int i = 0; env[i] != NULL; i++)
    {
        if (strstr(env[i], "PATH=/"))
        {
            printf("\n%s\n", env[i]);
            strcpy(dpath, env[i]);
            break;
        }
    }

    tokenize(&dpath[5], dir, colon, dpath, &ndir);

    // The base code assume only ONE dir[0] -> "/bin"
    // YOU do the general case of many dirs from PATH !!!!

    // show dirs
    for (i = 0; i < ndir; i++)
        printf("dir[%d] = %s\n", i, dir[i]);

    while (1)
    {
        printf("sh %d running\n", getpid());
        printf("enter a command line : ");
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        printf("line: %s", line);
        if (line[0] == 0)
            continue;

        tokenize(line, arg, space, gpath, &n);

        // printf("path[1]: %s", pathVars[1]);
        // printf("path[2]: %s", pathVars[2]);

        for (i = 0; i < n; i++)
        { // show token strings
            printf("arg[%d] = %s\n", i, arg[i]);
        }
        // getchar();

        cmd = arg[0]; // line = arg0 arg1 arg2 ...

        if (strcmp(cmd, "cd") == 0)
        {
            chdir(arg[1]);
            continue;
        }

        // int printFile(char *filePath)
        // {
        //     FILE *filePtr;
        //     char c;
        //     filePtr = fopen(filePath, "r");

        //     if (!filePtr)
        //     {
        //         printf("cannot cat file");
        //         return -1;
        //     }

        //     c = fgetc(filePtr);
        //     while (c != EOF)
        //     {
        //         putchar(c);
        //         c = fgetc(filePtr);
        //     }
        // }

        // if (strcmp(cmd, "cat") == 0)
        // {
        //     printFile(arg[1]);
        //     continue;
        // }

        // if (strcmp(cmd, "ls") == 0)
        // {
        //     DIR *cDir;
        //     struct dirent *cFile;

        //     if (!arg[1])
        //     {
        //         printf("hello");
        //         char cwd[PATH_MAX];
        //         getcwd(cwd, sizeof(cwd));
        //         cDir = opendir(cwd);
        //     }

        //     else
        //     {
        //         printf("wtf");
        //         cDir = opendir(arg[1]);
        //     }

        //     while (cFile = readdir(cDir))
        //     {
        //         char fPath[32] = "";
        //         strcpy(fPath, arg[1]);
        //         strcat(fPath, "/");
        //         strcat(fPath, cFile->d_name);
        //         struct stat info;
        //         stat(fPath, &info);
        //         fPath[0] = '\0';

        //         printf("%ld", info.st_size);
        //         printf("%s\n", cFile->d_name);
        //     }

        //     continue;
        // }
        //check

        if (strcmp(cmd, "exit") == 0)
            exit(0);

        pid = fork();

        if (pid)
        {
            printf("sh %d forked a child sh %d\n", getpid(), pid);
            printf("sh %d wait for child sh %d to terminate\n", getpid(), pid);
            pid = wait(&status);
            printf("ZOMBIE child=%d exitStatus=%x\n", pid, status);
            printf("main sh %d repeat loop\n", getpid());
        }
        else
        {
            printf("child sh %d running\n", getpid());

            // make a cmd line = dir[0]/cmd for execve()
            strcpy(line, dir[0]);
            strcat(line, "/");
            strcat(line, cmd);
            printf("line = %s\n", line);

            int r = execve(line, arg, env);

            printf("execve failed r = %d\n", r);
            exit(1);
        }
    }
}

/********************* YOU DO ***********************
1. I/O redirections:

Example: line = arg0 arg1 ... > argn-1

  check each arg[i]:
  if arg[i] = ">" {
     arg[i] = 0; // null terminated arg[ ] array 
     // do output redirection to arg[i+1] as in Page 131 of BOOK
  }
  Then execve() to change image


2. Pipes:

Single pipe   : cmd1 | cmd2 :  Chapter 3.10.3, 3.11.2

Multiple pipes: Chapter 3.11.2
****************************************************/
