#include <stdio.h>  // for I/O
#include <stdlib.h> // for I/O
#include <libgen.h> // for dirname()/basename()
#include <string.h>
#include <stdbool.h>

typedef struct node
{
  char name[64]; // node's name string
  char type;     // 'D' for DIR; 'F' for file
  struct node *child, *sibling, *parent;
} NODE;

NODE *root, *cwd, *start;
char line[128];
char command[16], pathname[64];

//               0       1      2      3      4       5
char *cmd[] = {"mkdir", "ls", "quit", "cd", "pwd", "rmdir", 0};

int findCmd(char *command)
{
  int i = 0;
  while (cmd[i])
  {
    if (strcmp(command, cmd[i]) == 0)
      return i;
    i++;
  }
  return -1;
}

NODE *search_child(NODE *parent, char *name)
{
  NODE *p;
  // printf("search for %s in parent DIR\n", name);
  p = parent->child;
  if (p == 0)
    return 0;
  while (p)
  {
    if (!strcmp(p->name, name))
      return p;
    p = p->sibling;
  }
  return 0;
}

int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  printf("insert NODE %s into END of parent child list\n", q->name);
  p = parent->child;
  if (p == 0)
    parent->child = q;
  else
  {
    while (p->sibling)
      p = p->sibling;
    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;
}

/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/

int mkdir(char *name)
{
  NODE *p, *q;
  printf("mkdir: name=%s\n", name);

  if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, ".."))
  {
    printf("can't mkdir with %s\n", name);
    return -1;
  }
  if (name[0] == '/')
    start = root;
  else
    start = cwd;

  printf("check whether %s already exists\n", name);
  p = search_child(start, name);
  if (p)
  {
    printf("name %s already exists, mkdir FAILED\n", name);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to mkdir %s\n", name);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'D';
  strcpy(q->name, name);
  insert_child(start, q);
  printf("mkdir %s OK\n", name);
  printf("--------------------------------------\n");

  return 0;
}

NODE *goto_path(char *pathname)
{
  int i = 0;
  int buffi = 0;
  char buffer[64] = "";
  NODE *current_dir;
  if (pathname[0] == '/')
    current_dir = root;
  else
    current_dir = cwd;

  while (pathname[i] != '\0')
  {
    // printf("%d\n", i);
    if (pathname[i] == '/')
    {
      current_dir = search_child(current_dir, buffer);
      buffer[0] = '\0';
      buffi = 0;
    }
    else
    {
      buffer[buffi] = pathname[i];
      // printf("%c\n", buffer[buffi]);
      // printf("%s\n", buffer);
      buffi++;
    }
    i++;
  }
  current_dir = search_child(current_dir, buffer);
  // printf("%s\n", buffer);
  return current_dir;
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char *pathname)
{
  NODE *p;
  if (!strcmp(pathname, ""))
  {
    p = cwd->child;
  }
  else
  {
    p = goto_path(pathname);
    printf("pathname = %s\n", p->name);
  }
  printf("cwd contents = ");
  while (p)
  {
    printf("[%c %s] ", p->type, p->name);
    p = p->sibling;
  }
  printf("\n");
}

int quit()
{
  printf("Program exit\n");
  exit(0);
  // improve quit() to SAVE the current tree as a Linux file
  // for reload the file to reconstruct the original tree
}

int initialize()
{
  root = (NODE *)malloc(sizeof(NODE));
  strcpy(root->name, "/");
  root->parent = root;
  root->sibling = 0;
  root->child = 0;
  root->type = 'D';
  cwd = root;
  printf("Root initialized OK\n");
}

// int find_dir(char *buffer, NODE *current_dir)
// {
//   while (current_dir)
//   {
//     if (current_dir->name == buffer)
//       return 0;
//   }
//   return -1;
// }

int cd(char *pathname)
{
  if (!strcmp(pathname, "/") || pathname[0] == '\0')
    cwd = root;
  else
    cwd = goto_path(pathname);
}

int pwd_help(NODE *current)
{
  if (current != root)
  {
    pwd_help(current->parent);
    printf("%s/", current->name);
  }
  else
    printf("%s", current->name);
}

int pwd()
{
  pwd_help(cwd);
  printf("\n");
}

int menu()
{
  printf("valid commands are:\n mkdir, rmdir, cd, ls, pwd, creat, rm, save, reload, menu, quit");
}

int removeFromList(NODE *pObject, char *pathname)
{
  NODE *current = pObject;
  printf("parent = %s", current->name);
  if (current->sibling)
  {
    current = current->child;
    while (current->sibling)
    {
      if (strcmp(current->sibling->name, pathname))
      {
        if (current->sibling->sibling)
          current->sibling = current->sibling->sibling;
        else
          current->sibling = NULL;
        return 0;
      }
      current = current->sibling;
    }
  }
  else
  {
    current->child = NULL;
    return 0;
  }
  return -1;
}

int rmdir(char *pathname)
{
  NODE *temp = goto_path(pathname);
  removeFromList(temp->parent, pathname);
  free(temp);
}

int save()
{
}

int reload()
{
}

int main()
{
  int index;

  initialize();

  printf("NOTE: commands = [mkdir|ls|quit|cd]\n");
  printf("cwd = %s child= %s", cwd->name, cwd->child->name);

  while (1)
  {
    printf("Enter command line : ");
    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;

    command[0] = pathname[0] = 0;
    sscanf(line, "%s %s", command, pathname);
    printf("command=%s pathname=%s\n", command, pathname);

    if (command[0] == 0)
      continue;

    index = findCmd(command);

    switch (index)
    {
    case 0:
      mkdir(pathname);
      break;
    case 1:
      ls(pathname);
      break;
    case 2:
      quit();
      break;
    case 3:
      cd(pathname);
      break;
    case 4:
      pwd();
      break;
    case 5:
      rmdir(pathname);
      break;
    }
  }
}
