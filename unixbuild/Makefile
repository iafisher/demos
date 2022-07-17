# Courtesy of https://tech.davis-hansson.com/p/make/
SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

CC := g++
CFLAGS := -Wall -Wextra -Werror -Iinclude -std=c++17

build: out/unixbuild
.PHONY: build

test: out/test
.PHONY: test

clean:
	rm -f out/*
.PHONY: clean

# This is a quick-and-dirty makefile and deps lists are not exhaustive, so you
# may need to sometimes run `make clean` to rebuild correctly.

out/unixbuild: src/*.cc
	$(CC) -o $@ $(CFLAGS) $^

out/test: test/*.cc src/common.cc
	$(CC) -o $@ $(CFLAGS) $^
	$@
