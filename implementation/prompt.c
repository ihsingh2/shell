#include "../headers/defs.h"

void prompt(metadata_t *vars)
{
  interface_t intf;
  refresh_intf(vars, &intf);
  
  fflush(stdout);

  printf("<" MAGENTA "%s@%s" RESET, intf.uname, intf.hname);
  printf(":" CYAN "%s" RESET, intf.rpath);
  if (vars->extime > EXTIME_BOUND)
    printf(":" YELLOW "%s~%ds" RESET, vars->last, (int) nearbyintf(vars->extime));
  printf("> ");

  fflush(stdout);
}

void refresh_intf(metadata_t *vars, interface_t *intf)
{
  getlogin_r(intf->uname, LOGIN_NAME_MAX);
  gethostname(intf->hname, HOST_NAME_MAX);
  getcwd(intf->cdir, PATH_MAX);
  intf->rpath[0] = 0;
  addtilde(intf->rpath, intf->cdir, vars->home);
}
