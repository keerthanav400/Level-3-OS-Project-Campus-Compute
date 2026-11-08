CC      = gcc
CFLAGS  = -Wall -O2 -std=gnu11 -pthread -Wno-unused-result -Wno-stringop-truncation -Wno-format-truncation
LDLIBS  = -pthread -lrt
SRCS    = main.c cli.c job.c scheduler.c logger.c ipc.c banker.c worker.c limits.c
OBJS    = $(SRCS:.c=.o)
BIN     = campus_compute

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN) campus_compute.log

.PHONY: all clean
