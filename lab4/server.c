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
#include <stdbool.h>

#define MAX 256
#define PORT 1234

int n;

char ans[MAX];
char line[MAX];

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

int ls(char *CWD, char *buffer)
{
  DIR *mydir;
  struct dirent *myfile;
  struct stat mystat;

  char buf[512];
  char buf2[300];
  mydir = opendir(CWD);
  while ((myfile = readdir(mydir)) != NULL)
  {
    sprintf(buf, "%s/%s", CWD, myfile->d_name);
    stat(buf, &mystat);
    sprintf(buf2, "%zu", mystat.st_size);
    strcat(buffer, buf2);
    sprintf(buf2, " %s\n", myfile->d_name);
    strcat(buffer, buf2);
  }
  closedir(mydir);

  printf("ls called\n");
  printf("trying to ls: %s\n", CWD);
}

int main()
{
  const int cSize = 8; //  0      1      2     3       4      5     6     7
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
      char *tokCommands[MAX] = {NULL};
      char pathname[MAX];
      char sendBuffer[MAX] = "";
      char CWD[MAX];
      char lineCpy[MAX];
      int tokNumber;
      int commandIndex;
      int fileReadSize;
      bool readPut;

      getcwd(CWD, sizeof(CWD));
      printf("server ready for next request ....\n");
      n = read(cfd, line, MAX);
      if (n == 0)
      {
        printf("server: client died, server loops\n");
        close(cfd);
        break;
      }

      // show the line string
      printf("server: read  n=%d bytes; line=[%s]\n", n, line);

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
        break;
      case 1:
        rmdir(pathname);
        break;
      case 2:
        unlink(pathname);
        break;
      case 3:
        chdir(pathname);
        break;
      case 4:
        //send back
        sprintf(sendBuffer, "\n%s\n", pathname);
        break;
      case 5:
        //send back
        ls(pathname, sendBuffer);
        printf("hello?");
        // printf("%s", sendBuffer);
        break;
      case 6:
        break;
      case 7:
        readPut = true;
        break;

      default:
        break;
      }

      // send the echo line to client
      strcat(line, "\n");
      strcat(line, sendBuffer);
      n = write(cfd, line, MAX);
      // write(cfd, sendBuffer, MAX);

      printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
      printf("server: ready for next request\n");

      if (readPut)
      {
        char sizeAsString[MAX];
        read(cfd, sizeAsString, MAX);
        // printf("read: %s", sizeAsString);

        sscanf(sizeAsString, "%d", &fileReadSize);
        char fileBuffer[fileReadSize];

        printf("file to put size = %dbytes", fileReadSize);

        write(cfd, line, MAX);

        read(cfd, fileBuffer, fileReadSize);

        FILE *fp;
        fp = fopen(pathname, "w");
        fprintf(fp, "%s", fileBuffer);
      }
    }
  }
}
