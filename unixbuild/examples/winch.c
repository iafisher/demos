/**
 * Waits for the terminal size to change, prints a message when it does, then
 * exits.
 */
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void sigwinch_handler(__attribute__((unused)) int signum) {
  const char* message = "Terminal size changed\n";
  write(STDOUT_FILENO, message, strlen(message));
}

int main() {
  struct sigaction act;
  act.sa_handler = sigwinch_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction(SIGWINCH, &act, NULL) < 0) {
    perror("sigaction()");
    return 1;
  }

  // Go to sleep until a signal is received.
  pause();
  return 0;
}
