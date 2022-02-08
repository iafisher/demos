A simple program to monitor a directory for changes using Linux's `inotify` interface.

```shell
make
./filewatch path/to/directory
```

It will run forever, terminating only if the directory it is watching is removed. Note that watching is not recursive: it will not detect changes to files in subdirectories.
