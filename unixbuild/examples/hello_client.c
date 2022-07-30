/**
 * A client to talk to the hello_server.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "hello.h"

int main() {
  int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket()");
    exit(1);
  }

  struct sockaddr_un un;
  memset(&un, 0, sizeof un);
  un.sun_family = AF_UNIX;
  strcpy(un.sun_path, HELLO_SOCKET_PATH);
  size_t size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
  if (connect(sockfd, (struct sockaddr*)&un, size) < 0) {
    perror("connect()");
    exit(1);
  }

  puts("Connected to server. Enter queries at prompt. Press Ctrl+D to quit.\n");

  char buf[HELLO_MESSAGE_SIZE];
  while (1) {
    printf("? ");
    char* r = fgets(buf, HELLO_MESSAGE_SIZE, stdin);
    if (r == NULL) {
      break;
    }
    size_t r_len = strlen(r);
    if (r[r_len - 1] == '\n') {
      r[r_len - 1] = '\0';
    }

    if (send(sockfd, buf, strlen(buf), 0) < 0) {
      perror("send()");
      exit(1);
    }

    ssize_t nread = recv(sockfd, buf, HELLO_MESSAGE_SIZE, 0);
    if (nread < 0) {
      perror("recv()");
      exit(1);
    }
    buf[nread] = '\0';
    printf("Received: %s\n", buf);
  }

  exit(0);
}
