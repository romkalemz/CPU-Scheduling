#include "inc.h"

// struct to hold necessary command line arguments
struct ARG {
    char algo[4];
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

    printf("algo: %s,   quantum: %i,    file name: %s\n", arg.algo, arg.quantum, arg.file_name);


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
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-alg") == 0) {
           // check if algo name exists
           if((strcmp(argv[i+1], "FIFO") != 0) && (strcmp(argv[i+1], "SJF") != 0)
           && (strcmp(argv[i+1], "PR") != 0) && (strcmp(argv[i+1], "RR") != 0)) {
                printf("\nERROR - INPROPER ARGUMENTS! -\n");
                printf("Algorithm name \"%s\" does not exist!\n", argv[i+1]);
                return 1;
           } else
                strcpy(arg.algo, argv[i+1]);
        } 
        else if(strcmp(argv[i], "-quantum") == 0) {
            // check if quantum tag an int input

        }
        else if(strcmp(argv[i], "-input") == 0) {
            // search for file name, and attempt to open to read
            arg.file_ptr = fopen(argv[i+1], "r");
            if(arg.file_ptr == NULL) {
                fprintf(stderr, "\nERROR - IMPROPER ARGUMENTS! -\n%s: %s\n\n", argv[i+1], strerror(errno));
                return 1;
            } else
                strcpy(arg.file_name, argv[i+1]);
        }
    }
    
    return 0;
}