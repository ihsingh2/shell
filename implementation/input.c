#include "../headers/defs.h"

int check_input(char *input, char *ret)
{
  if (ret == NULL)
  {
    if (feof(stdin)) {
      printf("\n");
      exit(0);
    }
    else {
      perror("fgets");
      return 0;
    }
  }
  
  if (input[0] == 0 || input[0] == '\r' || input[0] == '\n')
    return 0;

  input[strcspn(input, "\r\n")] = 0;
  return 1;
}

void handle_input(metadata_t *vars, char *input)
{
  add_pastevent(vars, input);

  char *saveptr;
  char *token = strtok_r(input, ";", &saveptr);
  while (token != NULL)
  {
    handle_command(vars, token);
    token = strtok_r(NULL, ";", &saveptr);
  }
}

void handle_command(metadata_t *vars, char *command)
{
  char *token = strchr(command, '&');
  while (token != NULL)
  {
    char job[token - command + 1];
    strncpy(job, command, token - command);
    job[token - command] = 0;
    if (!is_empty(job))
      handle_background_jobs(vars, job);
    command = token + 1;
    token = strchr(command, '&');
  }

  if (!is_empty(command))
    handle_foreground_jobs(vars, command);
}
