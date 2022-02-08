#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define BUFFER_LENGTH 4096

int handle_event(struct inotify_event*);

void error(const char*);
void usage(void);

int main(int argc, char* argv[]) {
    if (argc != 2 || argv[1][0] == '-') {
        usage();
        return 1;
    }

    int inotify_fd = inotify_init();
    int watch_fd = inotify_add_watch(inotify_fd, argv[1], IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVE);
    if (watch_fd == -1) {
        error("could not add file or directory to watchlist.");
    }
    printf("Watching %s for changes.\n", argv[1]);

    char buffer[BUFFER_LENGTH];
    while (1) {
        size_t nread = read(inotify_fd, buffer, BUFFER_LENGTH);
        for (char* p = buffer; p < buffer + nread; ) {
            struct inotify_event* event = (struct inotify_event*)p;
            if (handle_event(event) == -1) {
                puts("Terminating.");
                return 0;
            }
            p += sizeof(struct inotify_event) + event->len;
        }
    }
}

int handle_event(struct inotify_event* event) {
    if (event->mask & IN_CREATE) {
        printf("File was created: %s\n", event->name);
    } else if (event->mask & IN_DELETE) {
        printf("File was deleted: %s\n", event->name);
    } else if (event->mask & IN_DELETE_SELF) {
        return -1;
    } else if (event->mask & IN_MODIFY) {
        printf("File was modified: %s\n", event->name);
    } else if (event->mask & IN_MOVE) {
        printf("File was moved: %s\n", event->name);
    } else {
        puts("Unknown file event.");
    }

    return 0;
}

void error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

void usage() {
    puts("Usage: filewatch <file or directory to watch>");
}
