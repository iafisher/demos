/**
 * A simple multi-threaded local server. You can use the hello_client program to
 * communicate with it, e.g.
 *
 * client> ECHO hi
 * hi
 */
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "hello.h"

void service_connection(int fd);
void handle_message(int fd, const char* msg);
void send_error(int fd);

void sighandler(int signum);
void cleanup_full(void);
void cleanup_quick(void);

void error_and_bail(const char* msg);

// Global variable so that the signal handler can close it.
int sockfd = -1;

int main() {
  // Open the socket file descriptor.
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error_and_bail("socket()");
  }

  // Bind the socket to the address HELLO_SOCKET_PATH on the filesystem.
  struct sockaddr_un un;
  memset(&un, 0, sizeof un);
  un.sun_family = AF_UNIX;
  strcpy(un.sun_path, HELLO_SOCKET_PATH);
  size_t size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
  if (bind(sockfd, (struct sockaddr*)&un, size) < 0) {
    error_and_bail("bind()");
  }

  // Register clean-up function with signal handers and atexit.
  struct sigaction act;
  memset(&act, 0, sizeof act);
  act.sa_handler = sighandler;
  if (sigaction(SIGINT, &act, NULL) < 0) {
    error_and_bail("sigaction(SIGINT)");
  }

  if (sigaction(SIGTERM, &act, NULL) < 0) {
    error_and_bail("sigaction(SIGTERM)");
  }

  if (sigaction(SIGQUIT, &act, NULL) < 0) {
    error_and_bail("sigaction(SIGQUIT)");
  }

  if (atexit(cleanup_full) < 0) {
    error_and_bail("atexit()");
  }

  // Listen on the socket.
  if (listen(sockfd, 128) < 0) {
    error_and_bail("listen()");
  }

  printf("Listening for connections at %s (pid=%d)\n", HELLO_SOCKET_PATH,
         getpid());

  while (1) {
    // Accept connections forever.
    int connfd;
    if ((connfd = accept(sockfd, NULL, NULL)) < 0) {
      error_and_bail("accept()");
    }

    service_connection(connfd);
  }

  exit(EXIT_SUCCESS);
}

typedef struct {
  int fd;
  size_t index;
} thread_data_t;

#define MAX_THREADS 8
size_t thread_index = 0;
// These are global variables to ensure they live for the entire lifetime of the
// thread handler. `thread_data` should not be accessed directly by a thread, as
// it receives its own data as its argument.
pthread_t thread_pool[MAX_THREADS];
thread_data_t thread_data[MAX_THREADS];

void* thread_handler(void* arg) {
  thread_data_t* data = (thread_data_t*)arg;

  printf("Got a connection on thread %zu\n", data->index);
  char buf[HELLO_MESSAGE_SIZE + 1];

  while (1) {
    ssize_t msglen = recv(data->fd, buf, HELLO_MESSAGE_SIZE, 0);
    if (msglen == 0) {
      break;
    } else if (msglen < 0) {
      perror("recv()");
    }
    buf[msglen] = '\0';

    handle_message(data->fd, buf);
  }

  printf("Closing connection on thread %zu\n", data->index);
  close(data->fd);
  pthread_exit(NULL);
}

void service_connection(int fd) {
  if (thread_index >= MAX_THREADS) {
    // TODO: block until a thread is available.
    fputs("error: out of threads\n", stderr);
    send_error(fd);
    close(fd);
    return;
  }

  pthread_t* thread = thread_pool + thread_index;
  thread_data_t* data = thread_data + thread_index;
  data->fd = fd;
  data->index = thread_index;
  thread_index++;

  if (pthread_create(thread, NULL, thread_handler, data) < 0) {
    // TODO: don't exit here
    error_and_bail("pthread_create()");
  }
}

void handle_message(int fd, const char* msg) {
  char* end_of_cmd = strchr(msg, ' ');
  if (end_of_cmd == NULL) {
    send_error(fd);
    printf("Error: could not find command: %s\n", msg);
    return;
  } else {
    size_t n = end_of_cmd - msg;
    if (n == 4 && strncmp(msg, "ECHO", n) == 0) {
      const char* rest_of_msg = msg + n + 1;
      if (send(fd, rest_of_msg, strlen(rest_of_msg), 0) < 0) {
        perror("send()");
      }
    } else {
      send_error(fd);
      printf("Error: unknown command: %.*s\n", (int)n, msg);
      return;
    }

    printf("Received command: %.*s\n", (int)n, msg);
  }
}

void send_error(int fd) {
  const char* error = "error";
  if (send(fd, error, strlen(error), 0) < 0) {
    perror("send()");
  }
}

void sighandler(int signum) {
  if (signum == SIGQUIT) {
    cleanup_quick();
  } else {
    cleanup_full();
  }

  exit(0);
}

void cleanup_full() {
  const char* msg = "Shutting down\n";
  write(STDOUT_FILENO, msg, strlen(msg));

  cleanup_quick();
  unlink(HELLO_SOCKET_PATH);
}

void cleanup_quick() {
  if (sockfd != -1) {
    close(sockfd);
  }
}

void error_and_bail(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
