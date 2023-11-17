#include "../headers/defs.h"

void handle_warps(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL)
    return handle_warp(vars, "~");

  int i = 0;

  while (argv[++i] != NULL)
    handle_warp(vars, argv[i]);
}

void handle_warp(metadata_t *vars, char *arg)
{
  if (strcmp(arg, "-") == 0)
  {
    if (vars->oldpwd[0] != 0)
      execute_warp(vars, vars->oldpwd);
    else
      fprintf(stderr, "warp: oldpwd not set\n");
  }
  else
  {
    char path[PATH_MAX] = {0};
    remtilde(path, arg, vars->home);
    execute_warp(vars, path);
  }
}

void execute_warp(metadata_t *vars, char *arg)
{
  char cdir[PATH_MAX];
  getcwd(cdir, PATH_MAX);
  
  int ret = chdir(arg);
  if (ret != 0)
    perror("warp");
  else
  {
    strcpy(vars->oldpwd, cdir);
    getcwd(cdir, PATH_MAX);
    printf("%s\n", cdir);
  }
}
