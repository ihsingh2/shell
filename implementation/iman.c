#include "../headers/defs.h"

void handle_iman(metadata_t *vars, char **argv)
{
  if (argv[1] == NULL)
  {
    fprintf(stderr, "usage: iMan <command_name>\n");
    return;
  }

  execute_iman(vars, argv[1]);
}

void execute_iman(metadata_t *vars, char *name)
{
  const char *host = "man.he.net";
  const char *port = "80";
  int buffer_size = 8192;

  struct hostent *hostent = gethostbyname(host);
  if (hostent == NULL) {
    perror("iMan");
    return;
  }

  struct in_addr **addr_list = (struct in_addr **) hostent->h_addr_list;
  if (addr_list[0] == NULL) {
    perror("iMan");
    return;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("iMan");
    return;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(port));
  server_addr.sin_addr = *addr_list[0];

  if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    perror("iMan");
    return;
  }

  char request[CMD_SIZE];
  sprintf(request, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\n\r\n", name, host);
  if (send(sockfd, request, strlen(request), 0) == -1) {
    perror("iMan");
    return;
  }

  char buffer[buffer_size];
  char output[buffer_size];
  ssize_t bytes_received = recv(sockfd, buffer, buffer_size - 1, 0);
  if (bytes_received == -1) {
    perror("iMan");
    return;
  }

  buffer[bytes_received] = 0;
  if (strstr(buffer, "No matches for") != NULL) {
    fprintf(stderr, "ERROR\n       No such command\n");
    return;
  }

  char *body = strstr(buffer, "<body ");
  char *start = strstr(body, "NAME");
  char *end = strstr(start, "<center>");
  if (start != NULL) {
    if (end != NULL) {
      snprintf(output, end - start - 70, start);
      printf("%s\n", output);
    }
    else {
      printf("%s\n", start);
      while (1)
      {
        bytes_received = recv(sockfd, buffer, buffer_size - 1, 0);
        if (bytes_received == -1) {
          perror("iMan");
          break;
        }
        if (bytes_received == 0)
          break;

        buffer[bytes_received] = 0;

        if ((end = strstr(start, "<center>")) == NULL)
          printf("%s\n", buffer);
        else {
          if (end - 70 > buffer) {
            snprintf(output, end - buffer - 70, buffer);
            printf("%s\n", output);
          }
          break;
        }
      }
    }
  }
  else
    fprintf(stderr, "iMan: error parsing the body of man page");

  close(sockfd);
}
