#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

// struct to hold necessary command line arguments
struct ARG {
    char algo[5];
    int quantum;
    char file_name[120];
    FILE* file_ptr;
};

// (Process Control Block)
// struct that holds necessary process info found in the inputted file
struct PCB {
    int priority;
    int totalBursts;
    int *CPUburst, *IOBurst;    // stores cpu and io burst times read from file
    struct timespec ts_begin, ts_end;
    struct PCB *prev, *next;     // pointers to prev and next PCB in a double linked list
};

// main.c functions
int checkArgInput(int argc, char** argv);
void printPerformance();

// scheduling functions
void *fileRead(void *args);