# Package version number
PACKAGE_VERSION = 1.0

# Compiler commands and options
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c17
INCLUDE = -I src/
LDFLAGS = -L lib
AR = ar rcs
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

# Installation prefix
PREFIX = /usr/local

# Default Hash size
HASH_SIZE = 128

# Default locale for tests
# Use 'make test -e LOCALE=<YourLocale>' to override
LOCALE = en_GB.UTF-8

# OS Detection
OSFLAG :=
ifeq ($(OS), Windows_NT)
OSFLAG += -D WIN32
VALGRIND = ''
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG += -D LINUX -D _GNU_SOURCE
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG += -D MACOS
		VALGRIND =
	endif
endif

all: prereq libhash

prereq:
	mkdir -p bin lib

libhash: prereq bin/hash.o
	$(AR) lib/libvhash.a bin/hash.o

bin/hash.o: src/hash.*
	$(CC) $(CFLAGS) -c src/hash.c -D HASH_SIZE=$(HASH_SIZE) -o bin/hash.o $(OSFLAG)

# Installation targets

install: libhash
	$(eval INSTALL_LIB_DIR = $(PREFIX)/lib)
	$(eval INSTALL_INC_DIR = $(PREFIX)/include)
	if test ! -d "$(INSTALL_LIB_DIR)"; then mkdir -vp "$(INSTALL_LIB_DIR)"; fi \
	&& if test ! -d "$(INSTALL_INC_DIR)"; then mkdir -vp "$(INSTALL_INC_DIR)"; fi \
	&& cp lib/libvhash.a "$(INSTALL_LIB_DIR)/libvhash-$(PACKAGE_VERSION).a" \
	&& cp src/hash.h "$(INSTALL_INC_DIR)/vhash.h" \
	&& cd $(INSTALL_LIB_DIR) \
	&& ln -s libvhash-$(PACKAGE_VERSION).a libvhash.a

# Unit test targets

test: prereq prereq/debug libhash bin/main.o bin/hash_tests.o
	$(CC) $(CFLAGS) $(LDFLAGS) bin/main.o bin/hash_tests.o -lvhash -o bin/hash
	$(VALGRIND) bin/hash

prereq/debug:
	$(eval CFLAGS += -g)

bin/main.o:
	$(CC) $(CFLAGS) -c tests/main.c -DLOCALE='"$(LOCALE)"' $(INCLUDE) -o bin/main.o

bin/hash_tests.o: tests/hash_tests.*
	$(CC) $(CFLAGS) -c tests/hash_tests.c $(INCLUDE) -o bin/hash_tests.o $(OSFLAG)

# Other targets

# Creates a debug version of the library without running the tests
debug: clean prereq/debug all

clean:
	rm -rf bin/** lib/**

