#include "inc.h"

// struct to hold necessary command line arguments
struct ARG {
    char algo[5];
    int quantum;
    char file_name[120];
    FILE* file_ptr;
};
// global structure for easy access in main thread
struct ARG arg;

int main(int argc, char** argv) {

    // CHECK ARGV INPUT
    if(check_arg_input(argc, argv))
        // if an error detected, exit program
        exit(1);




    // print necessary statements after each thread finishes
    printf("\nInput File Name                 : %s\n", arg.file_name);
    printf("CPU Scheduling Algorithm        : %s", arg.algo);
    if(arg.quantum != 0)
        printf(" (%i ms)\n", arg.quantum);
    else
        printf("\n");
    printf("Avg. Turnaround time            : \n");
    printf("Avg. Waiting time in ready queue: \n\n");

    return 0;
}

int check_arg_input(int argc, char** argv) {
    // program must have "-alg [FIFO | SJF | PR | RR], 
    // [-quantum [integer(ms)]] -input [file_name]"
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