#include "../headers/defs.h"

struct termios orig_termios;

void handle_neonate(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL || strcmp(argv[1], "-n") != 0 || argv[2] == NULL)
  {
    fprintf(stderr, "usage: neonate -n <time_arg>\n");
    return;
  }

  int time = atoi(argv[2]);
  if (time < 0)
  {
    fprintf(stderr, "invalid option: %s\n", argv[2]);
    return;
  }

  execute_neonate(vars, time);
}

void execute_neonate(metadata_t *vars, int time)
{
  int rc = fork();
  if (rc < 0)
  {
    perror("fork");
    return;
  }
  else if (rc == 0)
  {
    print_recent_process(time);
    exit(0);
  }
  else
  {
    char c;
    while (1)
    {
      setbuf(stdout, NULL);
      enableRawMode();
      while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 'x')
        {
          kill(rc, SIGSTOP);
          disableRawMode();
          return;
        }
      }
      disableRawMode();
    }
  }
}

void print_recent_process(int time)
{
  FILE *fp;
  int pid;
  while (1) {
    fp = fopen("/proc/loadavg", "r");
    if (fp != NULL) {
      fscanf(fp, "%*f %*f %*f %*s %d", &pid);
      fclose(fp);
      printf("%d\n", pid);
    }
    else
      perror("neonate");
    sleep(time);
  }
}

void die(const char *s)
{
  perror(s);
  exit(1);
}

void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}
