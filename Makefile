CC = gcc
FILES = main.c schedulingEmulator.c
FLAGS = -Wall -Werror

prog: $(FILES)
	$(CC) -o prog $(FILES) $(FLAGS)