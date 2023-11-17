#include "../headers/defs.h"

void handle_seek(metadata_t *vars, char **argv)
{
  seekargs_t args;
  args.fname[0] = 0;
  args.dname[0] = 0;
  args.d_flag = 0;
  args.e_flag = 0;
  args.f_flag = 0;
  int i = 0;
  
  while (argv[++i] != NULL)
  {
    if (strcmp(argv[i], "-d") == 0)
      args.d_flag = 1;
    else if (strcmp(argv[i], "-e") == 0)
      args.e_flag = 1;
    else if (strcmp(argv[i], "-f") == 0)
      args.f_flag = 1;
    else if (strcmp(argv[i], "-de") == 0 || strcmp(argv[i], "-ed") == 0)
    {
      args.d_flag = 1;
      args.e_flag = 1;
    }
    else if (strcmp(argv[i], "-ef") == 0 || strcmp(argv[i], "-fe") == 0)
    {
      args.e_flag = 1;
      args.f_flag = 1;
    }
    else if (strcmp(argv[i], "-df") == 0 || strcmp(argv[i], "-fd") == 0 || strcmp(argv[i], "-efd") == 0 || strcmp(argv[i], "-fed") == 0 || strcmp(argv[i], "-fde") == 0 || strcmp(argv[i], "-edf") == 0 || strcmp(argv[i], "-def") == 0 || strcmp(argv[i], "-dfe") == 0)
    {
      fprintf(stderr, "seek: invalid flags");
      return;
    }
    else if (args.fname[0] == 0)
      strcpy(args.fname, argv[i]);
    else if (args.dname[0] == 0)
    {
      if (strcmp(argv[i], "-") == 0)
        strcpy(args.dname, vars->oldpwd);
      else
        remtilde(args.dname, argv[i], vars->home);
    }
    else
    {
      fprintf(stderr, "usage: seek <flags> <search> <target_dir>\n");
      return;
    }
  }

  if (args.fname[0] == 0) {
    fprintf(stderr, "usage: seek <flags> <search> <target_dir>\n");
    return;
  }

  if (args.dname[0] == 0)
    getcwd(args.dname, PATH_MAX);

  if (args.d_flag && args.f_flag)
  {
    fprintf(stderr, "seek: invalid flags");
    return;
  }

  execute_seek(vars, &args);
}

void execute_seek(metadata_t *vars, seekargs_t *args)
{
  struct timeval begin, end;
  gettimeofday(&begin, NULL);

  int found = 0;
  char path[PATH_MAX] = {0};
  search_directory(vars, args, &found, path, args->dname);

  if (found == 0)
    printf("seek: no match found\n");
    
  else if (found == 1 && args->e_flag)
  {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
      perror("stat");
      return;
    }

    if (S_ISDIR(statbuf.st_mode))
      execute_warp(vars, path);
    else
    {
      FILE *fp = fopen(path, "r");
      if (fp == NULL) {
        perror("seek");
        return;
      }
      else
      {
        char buffer[BLOCK_SIZE];
        size_t nread;

        while ((nread = fread(buffer, 1, BLOCK_SIZE, fp)) > 0)
          fwrite(buffer, 1, nread, stdout);

        fclose(fp);
      }
    }
  }

  gettimeofday(&end, NULL);
  vars->extime = end.tv_sec - begin.tv_sec;
  vars->extime += (end.tv_usec - begin.tv_usec) / 1000000.0;
  strcpy(vars->last, "seek");
}

void search_directory(metadata_t *vars, seekargs_t *args, int *found, char *path, char *cdir)
{
  DIR *dir = opendir(cdir);
  if (dir == NULL) {
    perror("opendir");
    return;
  }

  struct dirent *entry = readdir(dir);
  char entry_path[PATH_MAX];
  struct stat statbuf;

  while (entry != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      entry = readdir(dir);
      continue;
    }

    sprintf(entry_path, "%s/%s", cdir, entry->d_name);

    if (stat(entry_path, &statbuf) == -1) {
      perror("stat");
      entry = readdir(dir);
      continue;
    }

    if (S_ISDIR(statbuf.st_mode))
    {
      if (!args->f_flag && strstr(entry->d_name, args->fname) != NULL)
      {
        printf("%s\n", entry_path);
        if (*found == 0)
          strcpy(path, entry_path);
        (*found)++;
      }
      search_directory(vars, args, found, path, entry_path);
    }
    else if (!args->d_flag && strstr(entry->d_name, args->fname) != NULL)
    {
      printf("%s\n", entry_path);
      if (*found == 0)
        strcpy(path, entry_path);
      (*found)++;
    }

    entry = readdir(dir);
  }
  closedir(dir);
}
