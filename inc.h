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
#include <math.h>

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
    int ID;
    int priority;
    int totalBursts, numCPUBursts, numIOBursts;
    int totalBurstTime;         // all of the IO / CPU burst times added up
    int cpuIndex, ioIndex;
    int *CPUBurst, *IOBurst;    // stores cpu and io burst times read from file
    struct timespec ts_begin, ts_end;
    struct PCB *prev, *next;     // pointers to prev and next PCB in a double linked list
};
// global variables
extern sem_t sem_read, sem_cpu, sem_io;
extern struct PCB *ready_q_head, *io_q_head;
extern int file_read_done, cpu_sch_done;
extern int cpuBusy, ioBusy;
extern double elapsed, total_throughput, total_waiting_time, total_turnaround_time, total_num_processes;

// main.c functions
int checkArgInput(int argc, char** argv);
void printPerformance();

// scheduling functions
void *fileRead(void *args);
void *cpuSchedule();
void *ioSchedule();

// structure manipulation functions
struct PCB *createPCB(int ID);
void push(struct PCB **first, struct PCB *newPCB);
struct PCB *popQ(struct PCB **first);
int isEmptyQ(struct PCB *first);
void printQ(struct PCB **head);
void printNULL();
struct PCB *popSJF_or_popPR(struct PCB **head, int flag);