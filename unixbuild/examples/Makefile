# Courtesy of https://tech.davis-hansson.com/p/make/
SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

CC := gcc
CFLAGS := -Wall -Wextra -Werror

binaries = fork hello_client hello_server threads signals winch

all: $(binaries)

fork: fork.c
	$(CC) $(CFLAGS) -o $@ $<

hello_client: hello_client.c
	$(CC) $(CFLAGS) -o $@ $<

hello_server: hello_server.c
	$(CC) $(CFLAGS) -pthread -o $@ $<

signals: signals.c
	$(CC) $(CFLAGS) -o $@ $<

threads: threads.c
	$(CC) $(CFLAGS) -pthread -o $@ $<

winch: winch.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(binaries)
.PHONY: clean
