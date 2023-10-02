CC=go
AFLAGS=build -buildmode=c-archive
CFLAGS=build
SRCS=$(wildcard *.go)

TARGETS: ARCHIVE EXEC

ARCHIVE: $(SRCS)
	$(CC) $(AFLAGS)  $(SRCS)

EXEC: $(SRCS)
	$(CC) $(CFLAGS) -o secret $(SRCS)

clean:
	find ./ -maxdepth 1 -name "*.a" -delete
	find ./ -maxdepth 1 -name "secret" -delete

.PHONY: clean