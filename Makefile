CC = gcc

FILES = main.c fileReadThread.c cpuSchedulerThread.c ioSchedulerThread.c structManipulation.c
FLAGS = -Wall -Werror -pthread -lm

prog: $(FILES)
	$(CC) -o prog $(FILES) $(FLAGS)