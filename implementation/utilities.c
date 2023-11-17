#include "../headers/defs.h"

int is_empty(char *str)
{
  while (*str)
  {
    if (*str != ' ' && *str != '\t' && *str != '\r' && *str != '\n')
      return 0;
    str++;
  }
  return 1;
}

int count_occurrences(char *str, char target)
{
  int count = 0;
  while (*str)
  {
    if (*str == target)
      count++;
    str++;
  }
  return count;
}

void tokenize(char **argv, char *str, char *delim)
{
  int i = 0;
  char *saveptr;
  char *token = strtok_r(str, delim, &saveptr);
  while (token != NULL)
  {
    argv[i++] = token;
    token = strtok_r(NULL, delim, &saveptr);
  }
  argv[i] = NULL;
}

void tokenize2d(char *argv[][CMD_SIZE], char *str, char *delim1, char *delim2)
{
  int i = 0;
  char *saveptr1, *saveptr2;

  char *token1 = strtok_r(str, delim1, &saveptr1);
  while (token1 != NULL)
  {
    int j = 0;
    char *token2 = strtok_r(token1, delim2, &saveptr2);
    while (token2 != NULL)
    {
      argv[i][j++] = token2;
      token2 = strtok_r(NULL, delim2, &saveptr2);
    }
    argv[i++][j] = NULL;
    token1 = strtok_r(NULL, delim1, &saveptr1);
  }
}

int handle_redirection(metadata_t *vars, char **argv, int *in, int *out)
{
  int i = -1;
  int j = 0;
  char *new[CMD_SIZE];

  while (argv[++i] != NULL)
  {
    if (argv[i][0] == '>' && argv[i][1] != '>')
    {
      char path[PATH_MAX];
      if (argv[i][1] == 0)
        remtilde(path, argv[++i], vars->home);
      else
        remtilde(path, argv[i] + 1, vars->home);
      *out = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if (*out == -1) {
        perror("open");
        return 1;
      }
    }
    else if (argv[i][0] == '>' && argv[i][1] == '>')
    {
      char path[PATH_MAX];
      if (argv[i][2] == 0)
        remtilde(path, argv[++i], vars->home);
      else
        remtilde(path, argv[i] + 2, vars->home);
      *out = open(path, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if (*out == -1) {
        perror("open");
        return 1;
      }
    }
    else if (argv[i][0] == '<')
    {
      char path[PATH_MAX];
      if (argv[i][1] == 0)
        remtilde(path, argv[++i], vars->home);
      else
        remtilde(path, argv[i] + 1, vars->home);
      *in = open(path, O_RDONLY);
      if (*in == -1)
      {
        perror("open");
        return 1;
      }
    }
    else
      new[j++] = argv[i];
  }

  new[j] = NULL;

  for (int i = 0; i <= j; i++)
    argv[i] = new[i];

  return 0;
}

void addtilde(char *dest, char* src, char* home)
{
  char *base = src;
  while (*src && *home && (*src == *home))
  {
    src++;
    home++;
  }

  if (*home != 0)
    strcpy(dest, base);
  else
  {
    strcat(dest, "~");
    if (*src != 0)
      strcat(dest, src);
  }
}

void remtilde(char *dest, char* src, char* home)
{
  if (src[0] == '~')
  {
    strcat(dest, home);
    src++;
  }
  strcat(dest, src);
}

int modulo(int a, int m)
{
  int res = a % m;
  if (res < 0)
    res += m;
  return res;
}

int nmodulo(int a, int m)
{
  int res = a % m;
  if (res <= 0)
    res += m;
  return res;
}

int is_executable(char *dirname, char *path) {
  char full_path[PATH_MAX];
  sprintf(full_path, "%s/%s", dirname, path);
  
  struct stat file_stat;
  if (stat(full_path, &file_stat) != 0)
  {
    perror("stat failed");
    return 0;
  }

  return (file_stat.st_mode & S_IXUSR);
}

void get_permissions(char *perms, mode_t mode)
{
  strcpy(perms, "----------");
  if (S_ISDIR(mode)) perms[0] = 'd';
  if (mode & S_IRUSR) perms[1] = 'r';
  if (mode & S_IWUSR) perms[2] = 'w';
  if (mode & S_IXUSR) perms[3] = 'x';
  if (mode & S_IRGRP) perms[4] = 'r';
  if (mode & S_IWGRP) perms[5] = 'w';
  if (mode & S_IXGRP) perms[6] = 'x';
  if (mode & S_IROTH) perms[7] = 'r';
  if (mode & S_IWOTH) perms[8] = 'w';
  if (mode & S_IXOTH) perms[9] = 'x';
}
