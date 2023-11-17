#include "../headers/defs.h"

void handle_background_jobs(metadata_t *vars, char *job)
{
  char *found = strchr(job, '|');
  if (found == NULL)
    exec_bg_job(vars, job);
  else
    exec_piped_bg_job(vars, job);
}

void handle_foreground_jobs(metadata_t *vars, char *job)
{
  char *found = strchr(job, '|');
  if (found == NULL)
    exec_fg_job(vars, job);
  else
    exec_piped_fg_job(vars, job);
}

void check_background_jobs(metadata_t *vars)
{
  node_t *p = vars->proc.head;
  p = p->next;

  while (p != vars->proc.head)
  {
    char *cmd = p->inf.str;
    int pid = p->inf.val;
    
    int status;
    int ret = waitpid(pid, &status, WNOHANG);

    if (ret == -1) {
      perror("jobs");
      p = p->next;
    }
    else if (ret != 0)
    {
      if (WIFEXITED(status))
        printf("%d: %s exited normally.\n", pid, cmd);
      else
        printf("%d: %s exited abnormally.\n", pid, cmd);
      p = p->next;
      delete(&vars->proc, p->prev);
    }
    else
      p = p->next;
  }
}
