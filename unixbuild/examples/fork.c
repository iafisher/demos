#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;
  if ((pid = fork()) < 0) {
    perror("fork()");
    exit(1);
  } else if (pid == 0) {
    // child
    printf("[child] Created, with PID %d and parent PID %d\n", getpid(),
           getppid());
    puts("[child] Sleeping for 3 seconds");
    sleep(3);
    puts("[child] Woke up, now terminating");
  } else {
    // parent
    printf("[parent] Child process created with PID %d\n", pid);
    if (waitpid(pid, NULL, 0) < 0) {
      perror("waitpid()");
      exit(1);
    }
    puts("[parent] Terminating");
  }

  return 0;
}
