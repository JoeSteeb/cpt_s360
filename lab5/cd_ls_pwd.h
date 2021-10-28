/************* cd_ls_pwd.c file **************/
int cd()
{
  int ino = getino(pathname);
  MINODE *mip;

  // verify ino != 0
  if (!ino)
  {
    printf("cd: FAIL\n");
    return -1;
  }

  mip = iget(dev, ino);

  // verify mip->INODE is a directory
  if (!S_ISDIR(mip->INODE.i_mode))
  {
    printf("cd: FAIL\n");
    return -1;
  }

  iput(running->cwd); // release old cwd
  running->cwd = mip; // change cwd to mip

  printf("cd: OK\n");
  return 0;
}

int ls_dir(MINODE *mip)
{
  printf("ls_dir: list CWD's file names; YOU FINISH IT as ls -l\n");

  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;

  while (cp < buf + BLKSIZE)
  {
    struct stat *buf;
    buf = malloc(sizeof(struct stat));
    strncpy(temp, dp->name, dp->name_len);
    temp[dp->name_len] = 0;

    stat(temp, buf);

    // if (dp->pid == 0)
    // {
    //   printf("root\t");
    // }
    // else
    // {
    //   printf("user\t");
    // }

    printf("%ld\t", buf->st_size);
    printf("%ld\t", buf->st_mtim);
    printf("%s\n", temp);

    cp += dp->rec_len;
    dp = (DIR *)cp;
    free(buf);
  }
  printf("\n");
}

int ls_file(MINODE *mip, char *name)
{
  // printf("ls_file: to be done: READ textbook!!!!\n");
  // READ Chapter 11.7.3 HOW TO ls
  printf("hello");
  MINODE *curPath;
  int curIno = getino(pathname);
  curPath = iget(dev, curIno);

  if (S_ISDIR(curPath->INODE.i_mode))
  {
    ls_dir(curPath);
  }
  else
  {
    printf("ERROR: not a directory\n");
  }
}

int ls(char *pathname)
{
  // printf("ls: list CWD only! YOU FINISH IT for ls pathname\n");
  if (!pathname[0])
  {
    ls_dir(running->cwd);
  }
  else
  {
    ls_file(running->cwd, pathname);
  }
}

void rpwd(MINODE *wd)
{
  char buf[BLKSIZE], dirname[BLKSIZE];
  int my_ino, parent_ino;

  DIR *dp;
  char *cp;

  // parent minode
  MINODE *parentMINODE;

  if (wd == root)
    return;

  // get dir block of cwd
  get_block(wd->dev, wd->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;

  // search through cwd for my_ino and parent ino
  while (cp > buf + BLKSIZE)
  {
    strcpy(dirname, dp->name);
    dirname[dp->name_len] = '\0';

    // check for "." dir
    if (strcmp(dirname, ".") == 0)
    {
      my_ino = dp->inode;
    }

    // check for ".." dir
    if (strcmp(dirname, "..") == 0)
    {
      parent_ino = dp->inode;
    }

    // advance to next record
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }

  parentMINODE = iget(wd->dev, parent_ino);
  get_block(wd->dev, parentMINODE->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;

  while (cp < buf + BLKSIZE)
  {
    strncpy(dirname, dp->name, dp->name_len);
    dirname[dp->name_len] = 0;

    // check if we found directory associated with my_ino
    if (dp->inode == my_ino)
    {
      break;
    }

    // advance to next record
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  rpwd(parentMINODE);
  iput(parentMINODE);

  printf("/%s", dirname);
  return;
}

void pwd(MINODE *wd)
{
  if (wd == root)
  {
    printf("/\n");
    return;
  }

  rpwd(wd);
  printf("\n");
  return;
}
