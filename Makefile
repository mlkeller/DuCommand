CC = gcc
CFLAGS = -Wall -Werror -O3

DiskUsage : DiskUsage.c
	$(CC) $(CFLAGS) DiskUsage.c -o DiskUsage