#include "../headers/defs.h"

void handle_ping(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL || argv[2] == NULL) {
    fprintf(stderr, "usage: ping <pid> <sig_num>\n");
    return;
  }

  int pid, sig;
  pid = atoi(argv[1]);
  sig = atoi(argv[2]);

  int ret = kill(pid, modulo(sig, 32));
  if (ret = -1)
    perror("ping");
}
