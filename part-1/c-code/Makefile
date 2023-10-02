CC=gcc
LINKFLAGS=-shared
CFLAGS=-Wall
OBJFLAGS=-c -fPIC
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TARGET: hooklib.so

hooklib.so: $(OBJS)
	$(CC) $(LINKFLAGS) $^  -o $@

%.o: %.c %.h
	$(CC) $(OBJFLAGS) $< -o $@

clean:
	find ./ -maxdepth 1 -type f  -name "*.o" -delete
	find  ./ -maxdepth 1 -type f  -name "*.so" -delete
	
.PHONY: clean