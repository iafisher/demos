# Courtesy of https://tech.davis-hansson.com/p/make/
SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

CC := gcc
CFLAGS := -Wall -Wextra -Werror

binaries = fork threads signals

all: $(binaries)

fork: fork.c
	$(CC) $(CFLAGS) -o $@ $<

signals: signals.c
	$(CC) $(CFLAGS) -o $@ $<

threads: threads.c
	$(CC) $(CFLAGS) -pthread -o $@ $<

clean:
	rm -f $(binaries)
.PHONY: clean
