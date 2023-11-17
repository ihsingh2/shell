#include "../headers/defs.h"

void exec_piped_bg_job(metadata_t *vars, char *job)
{
  int rc, n = count_occurrences(job, '|');
  char *argv[n + 1][CMD_SIZE];
  tokenize2d(argv, job, "|", " \t");

  int pipes[n][2];
  for (int i = 0; i < n; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      return;
    }
  }

  for (int i = 0; i <= n; i++)
  {
    rc = fork();
    if (rc < 0)
    {
      perror("fork");
      return;
    }
    else if (rc == 0)
    {
      if (i > 0) {
        if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
          perror("dup2");
          return;
        }
      }

      if (i < n) {
        if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
          perror("dup2");
          return;
        }
      }

      for (int j = 0; j < n; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      if (strcmp(argv[i][0], "exit") == 0)
        exit(0);
      else if (strcmp(argv[i][0], "activities") == 0)
        handle_activities(vars, argv[i]);
      else if (strcmp(argv[i][0], "bg") == 0)
        handle_bg(vars, argv[i]);
      else if (strcmp(argv[i][0], "fg") == 0)
        handle_fg(vars, argv[i]);
      else if (strcmp(argv[i][0], "pastevents") == 0)
        handle_pastevents(vars, argv[i]);
      else if (strcmp(argv[i][0], "peek") == 0)
        handle_peek(vars, argv[i]);
      else if (strcmp(argv[i][0], "ping") == 0)
        handle_ping(vars, argv[i]);
      else if (strcmp(argv[i][0], "proclore") == 0)
        handle_proclore(vars, argv[i]);
      else if (strcmp(argv[i][0], "seek") == 0)
        handle_seek(vars, argv[i]);
      else if (strcmp(argv[i][0], "warp") == 0)
        handle_warps(vars, argv[i]);
      else {
        execvp(argv[i][0], argv[i]);
        if (errno == ENOENT)
          fprintf(stderr, "error: command not found\n");
        else
          perror("exec");
        exit(1);
      }
      exit(0);
    }
  }

  for (int i = 0; i < n; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }
}

void exec_piped_fg_job(metadata_t *vars, char *job)
{
  int n = count_occurrences(job, '|');
  int rc, pid[n + 1];
  
  strcpy(vars->last, job);
  char *argv[n + 1][CMD_SIZE];
  tokenize2d(argv, job, "|", " \t");

  int pipes[n][2];
  for (int i = 0; i < n; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      return;
    }
  }

  for (int i = 0; i <= n; i++)
  {
    rc = fork();
    if (rc < 0)
    {
      perror("fork");
      return;
    }
    else if (rc == 0)
    {
      if (i > 0) {
        if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
          perror("dup2");
          return;
        }
      }

      if (i < n) {
        if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
          perror("dup2");
          return;
        }
      }

      for (int j = 0; j < n; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      int in = -1, out = -1, fd_stdin, fd_stdout;
      if (handle_redirection(vars, argv[i], &in, &out) != 0)
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

      if (strcmp(argv[i][0], "exit") == 0)
        exit(0);
      else if (strcmp(argv[i][0], "activities") == 0)
        handle_activities(vars, argv[i]);
      else if (strcmp(argv[i][0], "bg") == 0)
        handle_bg(vars, argv[i]);
      else if (strcmp(argv[i][0], "fg") == 0)
        handle_fg(vars, argv[i]);
      else if (strcmp(argv[i][0], "iMan") == 0)
        handle_iman(vars, argv[i]);
      else if (strcmp(argv[i][0], "neonate") == 0)
        handle_neonate(vars, argv[i]);
      else if (strcmp(argv[i][0], "pastevents") == 0)
        handle_pastevents(vars, argv[i]);
      else if (strcmp(argv[i][0], "peek") == 0)
        handle_peek(vars, argv[i]);
      else if (strcmp(argv[i][0], "ping") == 0)
        handle_ping(vars, argv[i]);
      else if (strcmp(argv[i][0], "proclore") == 0)
        handle_proclore(vars, argv[i]);
      else if (strcmp(argv[i][0], "seek") == 0)
        handle_seek(vars, argv[i]);
      else if (strcmp(argv[i][0], "warp") == 0)
        handle_warps(vars, argv[i]);
      else
      {
        execvp(argv[i][0], argv[i]);

        if (errno == ENOENT)
          fprintf(stderr, "error: command not found\n");
        else
          perror("exec");

        exit(1);
      }

      if (in != -1) {
        dup2(fd_stdin, STDIN_FILENO);
        close(in);
      }

      if (out != -1) {
        dup2(fd_stdout, STDOUT_FILENO);
        close(out);
      }
      exit(0);
    }
    else
      pid[i] = rc;
  }

  for (int i = 0; i < n; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  for (int i = 0; i <= n; i++)
    waitpid(pid[i], NULL, WUNTRACED);
}
