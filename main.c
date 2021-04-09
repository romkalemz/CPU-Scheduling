#include "inc.h"

// global structure for easy access in main thread
struct ARG arg;
struct PCB *ready_q_head, *io_q_head;
sem_t sem_read, sem_cpu, sem_io;
int file_read_done, cpu_sch_done;
int cpuBusy, ioBusy;
double elapsed, total_throughput, total_waiting_time, total_turnaround_time, total_num_processes;
// struct timespec ts;
int main(int argc, char** argv) {
    int r;
    // CHECK COMMAND LINE ARGUMENTS
    if(checkArgInput(argc, argv))
        exit(1);    // if an error detected, exit program

    // CREATE / INIT DATA STRUCTURES (Ready_Q and IO_Q)
    ready_q_head = NULL;
    io_q_head = NULL;
    file_read_done = 0;
    cpu_sch_done = 0;
    cpuBusy = 0;
    ioBusy = 0;
    total_throughput = 0;
    total_waiting_time = 0;
    total_turnaround_time = 0;
    total_num_processes = 0;
    // CREATE THREE THREADS (FileRead_thread, CPU_scheduler_thread, IO_scheduler_thread)
    if (sem_init(&sem_read, 0, 1) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_init(&sem_cpu, 0, 0) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_init(&sem_io, 0, 0) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pthread_t thread_id[3];
    if ((r = pthread_create(&thread_id[0], NULL, fileRead, &arg)) != 0)
    {
        fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
        return EXIT_FAILURE;
    }
    if ((r = pthread_create(&thread_id[1], NULL, cpuSchedule, &arg)) != 0)
    {
        fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
        return EXIT_FAILURE;
    }
    if ((r = pthread_create(&thread_id[2], NULL, ioSchedule, NULL)) != 0)
    {
        fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
        return EXIT_FAILURE;
    }
    // join all threads/terminate
    for (int i = 0; i < 3; i++)
    {
        if ((r = pthread_join(thread_id[i], NULL)) == -1)
        {
            fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
            return EXIT_FAILURE;
        }
    }
    // destroy the semaphores
    if (sem_destroy(&sem_read) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_destroy(&sem_cpu) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_destroy(&sem_io) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    printPerformance();

    return 0;
}

// prints performance metrics generated from the emulated scheduler
void printPerformance() {
    printf("\nInput File Name                 : %s\n", arg.file_name);
    printf("CPU Scheduling Algorithm        : %s", arg.algo);
    if(arg.quantum != 0)
        printf(" (%i ms)\n", arg.quantum);
    else
        printf("\n");
    printf("Throughput                      : %.3lf ms\n", total_turnaround_time / 100);
    printf("Avg. Turnaround time            : %.1lf ms\n", (total_turnaround_time / total_num_processes) * 1000);
    printf("Avg. Waiting time in ready queue: %.3lf ms\n\n", (total_waiting_time / total_num_processes) * 100);
}

// checks the inputted command line arguments and parses necessary info
int checkArgInput(int argc, char** argv) {
    if(argc > 7 || argc < 5) {
        printf("\nERROR - INPROPER ARGUMENTS! -\n");
        printf("Please input: \"-alg [FIFO | SJF | PR | RR] [-quantum [integer(ms)]] -input [file_name]\"\n\n");
        return 1;
    }
    // loop through argv and set proper flag vars
    char end = '\0';    // used to end algo and file_name strings
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-alg") == 0) {
           // check if algo name exists
           if((strcmp(argv[i+1], "FIFO") != 0) && (strcmp(argv[i+1], "SJF") != 0)
           && (strcmp(argv[i+1], "PR") != 0) && (strcmp(argv[i+1], "RR") != 0)) {
                printf("\nERROR - INPROPER ARGUMENTS! -\n");
                printf("Algorithm name \"%s\" does not exist!\n", argv[i+1]);
                return 1;
           } else {
               // initialize algorithm that was chosen
                strcpy(arg.algo, argv[i+1]);
                strcat(arg.algo, &end);
           }
        } 
        else if(strcmp(argv[i], "-quantum") == 0) {
            // check if quantum tag an int, not a LONG_MIN, and not a LONG_MAX
            errno = 0;
            char* temp = argv[i+2]; // use a temp endptr to check if no numbers were detected
            arg.quantum = strtol(argv[i+1], &temp, 10); // convert string to int
            if(errno == EINVAL || errno == ERANGE || argv[i+1] == temp) {
                printf("\nERROR - INPROPER ARGUMENTS! -\n");
                printf("Quantum input \"%s\" is not a valid input!\n\n", argv[i+1]);
                return 1;
            }
        }
        else if(strcmp(argv[i], "-input") == 0) {
            // search for file name, and attempt to open to read
            arg.file_ptr = fopen(argv[i+1], "r");
            if(arg.file_ptr == NULL) {
                fprintf(stderr, "\nERROR - IMPROPER ARGUMENTS! -\n%s: %s\n\n", argv[i+1], strerror(errno));
                return 1;
            } else {
                strcpy(arg.file_name, argv[i+1]);
                strcat(arg.file_name, &end);
            }
        }
    }
    
    return 0;
}