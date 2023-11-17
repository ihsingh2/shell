#include "../headers/defs.h"

void init_pastevents(metadata_t *vars)
{
#ifdef PAST_PATH
  strcpy(vars->past.path, PAST_PATH);
#endif

#ifndef PAST_PATH
  char *str;
  if ((str = getenv("HOME")) == NULL)
    str = getpwuid(getuid())->pw_dir;
  strcat(vars->past.path, str);
  strcat(vars->past.path, "/.pastevents");
#endif

  for (int i = 0; i < PAST_SIZE; i++)
    vars->past.events[i][0] = 0;
  vars->past.head = 0;
  
  FILE *fp = fopen(vars->past.path, "r");

  if (fp != NULL)
  {
    while (vars->past.head < PAST_SIZE && fgets(vars->past.events[vars->past.head], CMD_SIZE, fp))
    {
      vars->past.events[vars->past.head][strcspn(vars->past.events[vars->past.head], "\r\n")] = 0;
      vars->past.head++;
    }
    vars->past.head %= PAST_SIZE;
    fclose(fp);
  }

  else
  {
    fp = fopen(vars->past.path, "w+");
    if (fp == NULL)
    {
      perror("pastevents");
      exit(0);
    }
    fclose(fp);
  }
}

void add_pastevent(metadata_t *vars, char* arg)
{
  int ret = process_pastevent(vars, arg);

  if (ret == 0)
  {
    if (strcmp(arg, vars->past.events[modulo(vars->past.head - 1, PAST_SIZE)]) == 0)
      return;

    strcpy(vars->past.events[vars->past.head], arg);
    vars->past.head++;
    vars->past.head %= PAST_SIZE;
    sync_pastevents(vars);
  }
}

int process_pastevent(metadata_t *vars, char* arg)
{
  char new[CMD_SIZE] = {0};
  int count = 0;
  int reps = 0;
  int flag = 0;

  char *ptr = strstr(arg, "pastevents");
  if (ptr == NULL)
    return 0;

  if (arg < ptr && *(ptr - 1) == ' ' || *(ptr - 1) == '\t')
    strncat(new, arg, ptr - arg - 1);
  else
    strncat(new, arg, ptr - arg);

  char *saveptr;
  char *token = strtok_r(ptr, " \t", &saveptr);
  if (arg == ptr && strcmp(token, "pastevents") == 0)
    flag = 1;

  while (token != NULL)
  {
    if (strcmp(token, "pastevents") == 0)
    {
      count++;
      token = strtok_r(NULL, " \t", &saveptr);
      if (token != NULL)
      {
        if (strcmp(token, "execute") == 0)
        {
          token = strtok_r(NULL, " \t", &saveptr);

          if (token != NULL)
          {
            count--;
            reps++;
            int i = atoi(token);
            if (!flag)
              strcat(new, " ");
            else
              flag = 0;
            strcat(new, vars->past.events[modulo(vars->past.head - i, PAST_SIZE)]);
            if (strchr(token, ';') != NULL)
              strcat(new, ";");
          }
          else
          {
            fprintf(stderr, "usage: pastevents execute {backward_index}\n");
            siglongjmp(vars->mark, 1);
          }
        }
        else
        {
          strcat(new, "pastevents ");
          strcat(new, token);
        }
      }
      else
      {
        strcat(new, "pastevents");
        break;
      }
    }
    else
    {
      if (strcmp(token, "pastevents;") == 0)
        count++;
      strcat(new, " ");
      strcat(new, token);
    }

    token = strtok_r(NULL, " \t", &saveptr);
  }

  strcpy(arg, new);

  if (reps)
    printf("%s\n", new);

  return count;
}

void sync_pastevents(metadata_t *vars)
{
  FILE *fp = fopen(vars->past.path, "w");

  for (int i = vars->past.head; i < PAST_SIZE; i++)
    if (vars->past.events[i][0] != 0)
      fprintf(fp, "%s\n", vars->past.events[i]);

  for (int i = 0; i < vars->past.head; i++)
    if (vars->past.events[i][0] != 0)
      fprintf(fp, "%s\n", vars->past.events[i]);

  fclose(fp);
}

void handle_pastevents(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL)
    display_pastevents(vars);
  else if (strcmp(argv[1], "purge") == 0)
    purge_pastevents(vars);
  else
    fprintf(stderr, "usage: pastevents </purge/execute {backward_index}>\n");
}

void display_pastevents(metadata_t *vars)
{
  for (int i = vars->past.head; i < PAST_SIZE; i++)
    if (vars->past.events[i][0] != 0)
      printf("[%d] %s\n", nmodulo(vars->past.head - i, PAST_SIZE), vars->past.events[i]);

  for (int i = 0; i < vars->past.head; i++)
    if (vars->past.events[i][0] != 0)
      printf("[%d] %s\n", nmodulo(vars->past.head - i, PAST_SIZE), vars->past.events[i]);
}

void purge_pastevents(metadata_t *vars)
{
  for (int i = 0; i < PAST_SIZE; i++)
    vars->past.events[i][0] = 0;
    
  FILE *fp = fopen(vars->past.path, "w");
  fclose(fp);
}
