UNAME := $(shell uname)
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L.

INC = include/
SRC = src/
PARENT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)


PARSER_SRC_FILES = $(wildcard src/GPX*.c)
PARSER_OBJ_FILES = $(patsubst src/GPX%.c,GPX%.o,$(PARSER_SRC_FILES))

ifeq ($(UNAME), Linux)
	XML_PATH = /usr/include/libxml2
endif
ifeq ($(UNAME), Darwin)
	XML_PATH = /System/Volumes/Data/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2
endif

all : release

parser: libgpxparser.so

libgpxparser.so: $(PARSER_OBJ_FILES) LinkedListAPI.o
	gcc -shared -o libgpxparser.so $(PARSER_OBJ_FILES) LinkedListAPI.o -lxml2 -lm

#Compiles all files named GPX*.c in src/ into object files, places all coresponding GPX*.o files in bin/
GPX%.o: $(SRC)GPX%.c $(INC)LinkedListAPI.h $(INC)GPX*.h
	gcc $(CFLAGS) -I$(XML_PATH) -I$(INC) -c -fpic $< -o $@

liblist.so: LinkedListAPI.o
	$(CC) -shared -o liblist.so LinkedListAPI.o

LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic -I$(INC) $(SRC)LinkedListAPI.c -o LinkedListAPI.o

RELEASE_DIR = $(PARENT_DIR)
RELEASE_FILES = libgpxparser.so 

release: $(RELEASE_FILES)
	for u in $(RELEASE_FILES); do echo $$u; cp -f $$u $(RELEASE_DIR); done

clean:
	rm -rf StructListDemo xmlExample *.o *.so

