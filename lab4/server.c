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
#include <libgen.h>
#include <time.h>

#define MAX 256
#define PORT 1234

int n;

char ans[MAX];
char line[MAX];

int tokenize(char *pathname, char *output[], char *token, int *num) // YOU have done this in LAB2
{                                                                   // YOU better know how to apply it from now on
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
  DIR *mydir;
  struct dirent *myfile;
  struct stat mystat;

  char buf[512];
  mydir = opendir(CWD);
  while ((myfile = readdir(mydir)) != NULL)
  {
    sprintf(buf, "%s/%s", CWD, myfile->d_name);
    stat(buf, &mystat);
    printf("%zu", mystat.st_size);
    printf(" %s\n", myfile->d_name);
  }
  closedir(mydir);
}

int main()
{
  const int cSize = 8;
  char *commands[] = {"mkdir", "rmdir", "rm", "cd", "pwd", "ls", "get", "put"};
  int sfd, cfd, len;
  struct sockaddr_in saddr, caddr;
  int i, length;

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
  //saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  saddr.sin_port = htons(PORT);

  printf("3. bind socket to server\n");
  if ((bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) != 0)
  {
    printf("socket bind failed\n");
    exit(0);
  }

  // Now server is ready to listen and verification
  if ((listen(sfd, 5)) != 0)
  {
    printf("Listen failed\n");
    exit(0);
  }
  while (1)
  {
    // Try to accept a client connection as descriptor newsock
    length = sizeof(caddr);
    cfd = accept(sfd, (struct sockaddr *)&caddr, &length);
    if (cfd < 0)
    {
      printf("server: accept error\n");
      exit(1);
    }

    printf("server: accepted a client connection from\n");
    printf("-----------------------------------------------\n");
    printf("    IP=%s  port=%d\n", "127.0.0.1", ntohs(caddr.sin_port));
    printf("-----------------------------------------------\n");

    // Processing loop
    while (1)
    {
      char *tokCommands[MAX];
      char pathname[MAX];
      char CWD[MAX];
      int tokNumber;
      int commandIndex;

      printf("server ready for next request ....\n");
      n = read(cfd, line, MAX);
      if (n == 0)
      {
        printf("server: client died, server loops\n");
        close(cfd);
        break;
      }

      tokenize(line, tokCommands, " ", &tokNumber);

      // printf("%s\n%s", tokCommands[0], tokCommands[1]);
      // printf("%s\n", CWD);

      strcpy(pathname, CWD);

      for (int i = 0; i < cSize; i++)
      {
        if (!strcmp(commands[i], tokCommands[0]))
        {
          commandIndex = i;
          if (tokCommands[1])
          {
            strcat(CWD, "/");
            strcat(pathname, tokCommands[1]);
          }
          break;
        }
      }
      printf("%s", pathname);

      switch (commandIndex)
      {
      case 0:
        mkdir(pathname, 0755);
        break;
      case 1:
        rmdir(pathname);
        break;
      case 2:
        unlink(pathname);
        break;
      case 4:
        chdir(pathname);
        break;
      case 5:
        ls(pathname);
        break;
      case 6:
        break;
      case 7:
        break;

      default:
        break;
      }

      // show the line string
      printf("server: read  n=%d bytes; line=[%s]\n", n, line);

      strcat(line, " ECHO");

      // send the echo line to client
      n = write(cfd, line, MAX);

      printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
      printf("server: ready for next request\n");
    }
  }
}
