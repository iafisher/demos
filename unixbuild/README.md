**WORK IN PROGRESS**

`unixbuild` is a simple build system that runs on Unix systems. It is intended to demonstrate many of the facilities available in the Unix programming environment, such as signals, sockets, job control, and more, in a large and somewhat realistic program. It is heavily indebted to the book *[Advanced Programming in the UNIX Environment](https://www.oreilly.com/library/view/advanced-programming-in/9780321638014/)* by W. Richard Stevens and Stephen A. Rago.

# Usage
```shell
# Build the default target.
$ unixbuild BUILD.uxb

# Build a specific target by name.
$ unixbuild BUILD.uxb mylib

# Specify the output directory.
$ unixbuild BUILD.uxb --out obj
```

## Build file format
A `BUILD.uxb` file somewhat resembles a makefile, without the ability to configure parameters or to explicitly specify the build commands. Each line should have the name of an output file, followed by a colon and a space-separated list of the files that it depends on.

```
hello: hello.c include/mylib.h mylib.o
mylib.o: mylib.c

# Comments begin with a pound mark.
```

`unixbuild` deduces the correct GCC invocation based on the form of the output and dependencies. If the output has the `.o` extension, `unixbuild` will produce an object file. Otherwise, it will produce an executable. Any header files that are included as dependencies will cause `unixbuild` to add the header file's directory to GCC's `include` search path. If a dependent file does not exist, `unixbuild` will look for a rule to produce it in the build file, and invoke that rule first. The dependent files must be listed literally; `unixbuild` will not interpret glob patterns.

`unixbuild` will only rebuild a file if any of its direct or indirect dependencies have changed, i.e. have a newer modified timestamp than the output file.

# Design
`unixbuild` consists of a client program that parses the command-line arguments, and a daemon process that does most of the heavy lifting. The daemon process is started automatically by the client if it is not running. A daemon is used so that the parsing and analysis of `BUILD.uxb` files can be cached in memory and reused by separate invocations of the `unixbuild` command.

# Development
Building `unixbuild` from source requires Make and a version of gcc capable of building C++17 code.

To build the executable:

```shell
$ make
```

To run the test suite:

```shell
$ make test
```
