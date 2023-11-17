#include "../headers/defs.h"

void handle_proclore(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL)
    return execute_proclore(vars, getpid());
  
  execute_proclore(vars, atoi(argv[1]));
}

void execute_proclore(metadata_t *vars, int arg)
{
  char path[PATH_MAX];
  char buffer[PATH_MAX];
  FILE *fp;
  char state;
  int pid, pgrp, tpgid;
  unsigned long vsize;

  sprintf(path, "/proc/%d/stat", arg);
  fp = fopen(path, "r");
  if (fp != NULL)
  {
    fscanf(fp, "%d %*s %c %*d %d %*d %*d %d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %lu",
           &pid, &state, &pgrp, &tpgid, &vsize);
    fclose(fp);
    printf("PID: %d\n", pid);
    printf("Process State: %c", state);
    if (pid == tpgid)
      printf("+");
    printf("\n");
    printf("Process Group: %d\n", pgrp);
    printf("Virtual Memory: %d\n", vsize);
  }
  else
    perror("proclore");

  sprintf(path, "/proc/%d/exe", arg);
  fp = fopen(path, "r");
  ssize_t length = readlink(path, buffer, PATH_MAX);
  if (length > 0)
  {
    buffer[length] = 0;
    printf("Executable Path: %s\n", buffer);
  }
  else
    perror("proclore");
}
