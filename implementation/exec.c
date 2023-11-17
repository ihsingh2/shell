#include "../headers/defs.h"

void exec_bg_job(metadata_t *vars, char *job)
{
  int n = count_occurrences(job, '|');
  if (n > 0)
    return exec_piped_bg_job(vars, job);

  strcpy(vars->running.str, job);
  char *argv[CMD_SIZE];
  tokenize(argv, job, " \t");

  int rc = fork();
  if (rc < 0)
  {
    perror("fork");
    return;
  }
  else if (rc == 0)
  {
    int in = -1, out = -1, fd_stdin, fd_stdout;
    if (handle_redirection(vars, argv, &in, &out) != 0)
      exit(1);

    if (in != -1) {
      fd_stdin = dup(STDIN_FILENO);
      if (dup2(in, STDIN_FILENO) == -1) {
        perror("dup2");
        exit(1);
      }
    }
    
    if (out != -1) {
      fd_stdout = dup(STDOUT_FILENO);
      if (dup2(out, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
      }
    }

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    
    execvp(argv[0], argv);
    if (errno == ENOENT)
      fprintf(stderr, "error: command not found\n");
    else
      perror("exec");
      
    exit(1);
  }
  else
  {
    insert(&vars->proc, vars->running.str, rc);
    printf("%d\n", rc);
  }
}

void exec_fg_job(metadata_t *vars, char *job)
{
  int n = count_occurrences(job, '|');
  if (n > 0)
    return exec_piped_bg_job(vars, job);

  strcpy(vars->running.str, job);
  char *argv[CMD_SIZE];
  tokenize(argv, job, " \t");

  int in = -1, out = -1, fd_stdin, fd_stdout;
  if (handle_redirection(vars, argv, &in, &out) != 0)
    return;

  if (in != -1) {
    fd_stdin = dup(STDIN_FILENO);
    if (dup2(in, STDIN_FILENO) == -1) {
      perror("dup2");
      return;
    }
  }
  
  if (out != -1) {
    fd_stdout = dup(STDOUT_FILENO);
    if (dup2(out, STDOUT_FILENO) == -1) {
      perror("dup2");
      return;
    }
  }

  struct timeval begin, end;
  gettimeofday(&begin, NULL);

  if (strcmp(argv[0], "exit") == 0)
    exit(0);
  else if (strcmp(argv[0], "activities") == 0)
    handle_activities(vars, argv);
  else if (strcmp(argv[0], "bg") == 0)
    handle_bg(vars, argv);
  else if (strcmp(argv[0], "fg") == 0)
    handle_fg(vars, argv);
  else if (strcmp(argv[0], "iMan") == 0)
    handle_iman(vars, argv);
  else if (strcmp(argv[0], "neonate") == 0)
    handle_neonate(vars, argv);
  else if (strcmp(argv[0], "pastevents") == 0)
    handle_pastevents(vars, argv);
  else if (strcmp(argv[0], "peek") == 0)
    handle_peek(vars, argv);
  else if (strcmp(argv[0], "ping") == 0)
    handle_ping(vars, argv);
  else if (strcmp(argv[0], "proclore") == 0)
    handle_proclore(vars, argv);
  else if (strcmp(argv[0], "seek") == 0)
    handle_seek(vars, argv);
  else if (strcmp(argv[0], "warp") == 0)
    handle_warps(vars, argv);
  else
  {
    int rc = fork();
    if (rc < 0)
    {
      perror("fork");
      return;
    }
    else if (rc == 0)
    {
      execvp(argv[0], argv);

      if (errno == ENOENT)
        fprintf(stderr, "error: command not found\n");
      else
        perror("exec");

      exit(1);
    }
    else
    {
      vars->running.val = rc;
    
      waitpid(rc, NULL, WUNTRACED);

      gettimeofday(&end, NULL);
      vars->extime = end.tv_sec - begin.tv_sec;
      vars->extime += (end.tv_usec - begin.tv_usec) / 1000000.0;
      strcpy(vars->last, vars->running.str);
      vars->running.val = -1;
      
      return;
    }
  }

  if (in != -1) {
    dup2(fd_stdin, STDIN_FILENO);
    close(in);
  }

  if (out != -1) {
    dup2(fd_stdout, STDOUT_FILENO);
    close(out);
  }

  gettimeofday(&end, NULL);
  vars->extime = end.tv_sec - begin.tv_sec;
  vars->extime += (end.tv_usec - begin.tv_usec) / 1000000.0;
  strcpy(vars->last, vars->running.str);

}
