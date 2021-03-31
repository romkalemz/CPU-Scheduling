CC = gcc
FILES = main.c scheduling-algorithms.c
FLAGS = -Wall -Werror

prog: $(FILES)
	$(CC) -o prog $(FILES) $(FLAGS)