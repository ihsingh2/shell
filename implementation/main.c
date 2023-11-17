#include "../headers/defs.h"

metadata_t vars;

int main(void)
{
  char *ret;
  init_env();
  signal(SIGINT, int_handler);
  signal(SIGTSTP, tstp_handler);

  while (1)
  {
    if (sigsetjmp(vars.mark, 1) != 0)
      printf("\n");
    else
    {
      prompt(&vars);
      refresh_env();
      
      char input[CMD_SIZE];
      ret = fgets(input, CMD_SIZE, stdin);

      check_background_jobs(&vars);

      if (check_input(input, ret))
        handle_input(&vars, input);
    }
  }
}

void init_env(void)
{
  getcwd(vars.home, PATH_MAX);
  vars.oldpwd[0] = 0;
  vars.last[0] = 0;
  float extime = 0;
  vars.running.str[0] = 0;
  vars.running.val = -1;
  init_pastevents(&vars);
  init_list(&vars.proc);
}

void refresh_env(void)
{
  vars.last[0] = 0;
  vars.extime = 0;
  vars.running.str[0] = 0;
  vars.running.val = -1;
}

void int_handler(int sig)
{
  if (vars.running.val != -1)
    kill(vars.running.val, SIGKILL);
  vars.running.val = -1;
  siglongjmp(vars.mark, 1);
}

void tstp_handler(int sig)
{
  if (vars.running.val != -1) {
    kill(vars.running.val, SIGSTOP);
    insert(&vars.proc, vars.running.str, vars.running.val);
    printf("\n");
    printf("%d: %s was stopped.\n", vars.running.val, vars.running.str);
    vars.running.val = -1;
  }
}
