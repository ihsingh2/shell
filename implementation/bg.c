#include "../headers/defs.h"

void handle_bg(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL) {
    fprintf(stderr, "usage: bg <pid>\n");
    return;
  }

  int pid = atoi(argv[1]);
  node_t *ret = search(&vars->proc, pid);
  if (ret == NULL) {
    fprintf(stderr, "bg: no such process found\n");
    return;
  }

  printf("%s &\n", ret->inf.str);
  kill(pid, SIGCONT);
}
