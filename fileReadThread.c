#include "inc.h"

// FileRead_thread
// Read the inputted file, create (emulated) process, place in Ready_Q
void *fileRead(void *args) {
    struct ARG *arg = args;
    // read first line
    char *lineptr = NULL;
    size_t linenum = 0;
    getline(&lineptr, &linenum, arg->file_ptr);

    // while NOT at the end of file...
    // ... read each line and process
    //while(1 == 1) {
        // read first line
        // if the command is proc
            // create PCB structure
            // read parameters into struct
            // insert PCB into Ready_Q
            // sem_post ???

        // if the command is sleep
            // sleep the given ms

        // if the command is stop
            // break
    //}
    return 0;
}