CC=gcc
CFLAGS=-Wall -Wextra -pthread -std=c99

# Collect all object/source modules across Teammate 1 and Teammate 2
SRCS = main.c job.c scheduler.c logger.c ipc.c banker.c worker.c limits.c cli.c
OBJS = $(SRCS:.c=.o)

TARGET = campus_compute

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o *.log /dev/shm/campus_compute_shm /dev/shm/sem.campus_compute_sem

.PHONY: all clean
