#include <fcntl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "unixbuild/common.h"

void daemon_startup(void);

int main() {
  try {
    daemon_startup();

    syslog(LOG_INFO, "server started");
    sleep(30);
  } catch (unixbuild::ExitException& e) {
    syslog(LOG_ERR, "%s", e.message_.c_str());
    return e.returncode_;
  }
  return 0;
}

void daemon_startup() {
  // Daemon start-up steps, adapted from chapter 13 of Advanced Programming in
  // the UNIX Environment.

  umask(0);

  // Our goal is to avoid acquiring a terminal, since we are meant to be daemon
  // process. We assume that we were fork()ed from a client process that was run
  // as a shell command. setsid() makes us the leader of a new session and of a
  // new process group without a controlling terminal.
  if (setsid() < 0) {
    throw unixbuild::ExitException("setsid() returned an error status", 1);
  }

  // Close all open file descriptors.
  struct rlimit rl;
  if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
    throw unixbuild::ExitException("getrlimit() returned an error status", 1);
  }
  if (rl.rlim_max == RLIM_INFINITY) {
    rl.rlim_max = 1024;
  }
  for (rlim_t i = 0; i < rl.rlim_max; i++) {
    close(i);
  }

  // Attach the file descriptors for standard input, output, and error to
  // /dev/null.
  open("/dev/null", O_RDWR);
  dup(0);
  dup(0);

  // Initialize syslog.
  openlog("unixbuild-server", LOG_CONS, LOG_DAEMON);
}
