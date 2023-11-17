#include "../headers/defs.h"

void handle_fg(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL) {
    fprintf(stderr, "usage: fg <pid>\n");
    return;
  }

  int pid = atoi(argv[1]);
  node_t *ret = search(&vars->proc, pid);
  if (ret == NULL) {
    fprintf(stderr, "fg: no such process found\n");
    return;
  }

  printf("%s\n", ret->inf.str);
  kill(pid, SIGCONT);
  delete(&vars->proc, ret);

  strcpy(vars->running.str, ret->inf.str);
  vars->running.val = pid;

  struct timeval begin, end;
  gettimeofday(&begin, NULL);
  waitpid(pid, NULL, WUNTRACED);
  gettimeofday(&end, NULL);

  vars->extime = end.tv_sec - begin.tv_sec;
  vars->extime += (end.tv_usec - begin.tv_usec) / 1000000.0;
  strcpy(vars->last, ret->inf.str);

  vars->running.val = -1;
}
