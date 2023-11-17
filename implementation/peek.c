#include "../headers/defs.h"

void handle_peek(metadata_t *vars, char **argv)
{
  char dname[PATH_MAX] = {0};
  int a_flag = 0;
  int l_flag = 0;
  int i = 0;

  while (argv[++i] != NULL)
  {
    if (strcmp(argv[i], "-a") == 0)
      a_flag = 1;
    else if (strcmp(argv[i], "-l") == 0)
      l_flag = 1;
    else if (strcmp(argv[i], "-la") == 0)
    {
      l_flag = 1;
      a_flag = 1;
    }
    else if (strcmp(argv[i], "-al") == 0)
    {
      a_flag = 1;
      l_flag = 1;
    }
    else if (dname[0] == 0)
    {
      if (strcmp(argv[i], "-") == 0)
        strcpy(dname, vars->oldpwd);
      else
        remtilde(dname, argv[i], vars->home);
    }
  }

  if (dname[0] == 0)
    getcwd(dname, PATH_MAX);

  if (l_flag)
    execute_peek_longlist(vars, dname, a_flag);
  else
    execute_peek(vars, dname, a_flag);
}

void execute_peek(metadata_t *vars, char *dname, int a_flag)
{
  DIR *dir = opendir(dname);
  if (dir == NULL) {
    perror("peek");
    return;
  }
  
  struct dirent **namelist;

  int entries = scandir(dname, &namelist, 0, alphasort);
  if (entries < 0) {
    perror("peek");
    return;
  }

  for (int i = 0; i < entries; i++)
  {
    if (a_flag || namelist[i]->d_name[0] != '.')
    {
      if (namelist[i]->d_type == DT_DIR)
        printf(BLUE);
      else if (is_executable(dname, namelist[i]->d_name))
        printf(GREEN);
      printf("%s\n", namelist[i]->d_name);
      printf(RESET);
    }
    free(namelist[i]);
  }
  free(namelist);
  closedir(dir);
}

void execute_peek_longlist(metadata_t *vars, char *dname, int a_flag)
{
  DIR *dir = opendir(dname);
  if (dir == NULL) {
    perror("peek");
    return;
  }

  struct dirent **namelist;

  int entries = scandir(dname, &namelist, 0, alphasort);
  if (entries < 0) {
    perror("peek");
    return;
  }

  for (int i = 0; i < entries; i++)
  {
    if (a_flag || namelist[i]->d_name[0] != '.')
    {
      struct stat statbuf;
      char full_path[PATH_MAX];
      sprintf(full_path, "%s/%s", dname, namelist[i]->d_name);

      if (stat(full_path, &statbuf) != 0) {
        perror("peek");
        free(namelist[i]);
        continue;
      }

      struct passwd *user = getpwuid(statbuf.st_uid);
      struct group *group = getgrgid(statbuf.st_gid);

      char time_str[80];
      strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&statbuf.st_mtime));

      char perms[11];
      get_permissions(perms, statbuf.st_mode);

      printf("%s %2zu %-8s %-8s %8zu %s ",
        perms,
        statbuf.st_nlink,
        user->pw_name,
        group->gr_name,
        statbuf.st_size,
        time_str);

      if (namelist[i]->d_type == DT_DIR)
        printf(BLUE);
      else if (is_executable(dname, namelist[i]->d_name))
        printf(GREEN);
      printf("%s\n", namelist[i]->d_name);
      printf(RESET);
    }
    free(namelist[i]);
  }
  free(namelist);
  closedir(dir);
}
