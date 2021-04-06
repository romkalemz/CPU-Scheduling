CC = gcc
FILES = main.c fileReadThread.c cpuSchedulerThread.c ioSchedulerThread.c
FLAGS = -Wall -Werror -pthread

prog: $(FILES)
	$(CC) -o prog $(FILES) $(FLAGS)