.PHONY: clean

CFLAGS= -std=c17 -pedantic -Wall -O3 -g -c

CC = gcc
LINKER = $(CC)
NASM = nasm

SOLUTION = solution

TESTER = ./tester
TESTS = $(TESTER)/tests

SRC = $(SOLUTION)/src
OBJECT = obj
TARGET = build

MKDIR = mkdir -p

PROGRAM = ./$(TARGET)/main
IMAGE_TESTER = ./$(TARGET)/image-tester

all: $(OBJECT) $(TARGET)
	$(NASM) -felf64 -g $(SRC)/transform/sepia_filter_sse.asm -o $(OBJECT)/sepia_filter_sse.o

	$(CC) $(CFLAGS) $(SRC)/bmp/bmp_padding.c -o $(OBJECT)/bmp_padding.o
	$(CC) $(CFLAGS) $(SRC)/bmp/bmp.c -o $(OBJECT)/bmp.o
	$(CC) $(CFLAGS) $(SRC)/bmp/bmp_reader.c -o $(OBJECT)/bmp_reader.o
	$(CC) $(CFLAGS) $(SRC)/file/file.c -o $(OBJECT)/file.o
	$(CC) $(CFLAGS) $(SRC)/img/image.c -o $(OBJECT)/image.o
	$(CC) $(CFLAGS) $(SRC)/transform/rotate.c -o $(OBJECT)/rotate.o
	$(CC) $(CFLAGS) $(SRC)/transform/sepia_filter.c -o $(OBJECT)/sepia_filter.o
	$(CC) $(CFLAGS) $(SRC)/main.c -o $(OBJECT)/main.o
	$(LINKER) -no-pie -g $(OBJECT)/*.o -o  $(PROGRAM)

$(OBJECT):
	$(MKDIR) $@

$(TARGET):
	$(MKDIR) $@

clean:
	rm -rf $(OBJECT) $(TARGET)

perf-tester:
	$(PROGRAM) -perf  ./tests/4/input.bmp
