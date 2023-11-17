#include "../headers/defs.h"

void handle_activities(metadata_t *vars, char **argv)
{
  node_t *p = vars->proc.head->next;
  while (p != vars->proc.head)
  {
    print_process_info(p->inf.str, p->inf.val);
    p = p->next;
  }
}

void print_process_info(char *cmd, int pid)
{
  char path[PATH_MAX];
  sprintf(path, "/proc/%d/stat", pid);
  FILE *fp = fopen(path, "r");

  if (fp != NULL)
  {
    char state;
    fscanf(fp, "%*d %*s %c", &state);
    fclose(fp);

    printf("%d: %s - ", pid, cmd);
    if (state == 'T')
      printf("stopped\n");
    else
      printf("running\n");
  }
  else
    perror("activities");
}
