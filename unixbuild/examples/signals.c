/**
 * A simple program that sleeps for 10 seconds and refuses to be killed by
 * SIGINT. This is not a good idea in general but it serves as a convenient
 * demonstration since it's easy to send SIGINT by pressing Ctrl+C in the shell.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void do_not_kill_me(int);

int main() {
  sigset_t sigset;
  if (sigemptyset(&sigset) < 0) {
    perror("sigemptyset()");
    exit(1);
  }

  struct sigaction act = {
      .sa_handler = do_not_kill_me,
      .sa_mask = sigset,
      .sa_flags = 0,
      .sa_restorer = NULL,
  };
  sigaction(SIGINT, &act, NULL);

  // If we do one 10-second sleep, then the signal will interrupt the sleep and
  // cause `main` to return immediately. But if we do 10 one-second sleeps, then
  // only one second of the sleep will be interrupted and the rest will continue
  // normally until exit.
  for (int i = 0; i < 10; i++) {
    sleep(1);
  }

  return 0;
}

void do_not_kill_me(__attribute__((unused)) int signum) {
  const char* msg = "I refuse to die!\n";
  // POSIX guarantees that `write` is safe to call from a signal handler; other
  // I/O functions like `printf` are not safe because they are not reentrant.
  write(STDOUT_FILENO, msg, strlen(msg));
}
