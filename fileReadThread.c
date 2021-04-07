#include "inc.h"

// FileRead_thread
// Read the inputted file, create (emulated) process, place in Ready_Q
void *fileRead(void *args) {

    struct ARG *arg = args;
    struct PCB *curr_struct;
    // read first line
    char *word, *lineptr = NULL;
    size_t linenum = 0;
    ssize_t num_chars_inline = getline(&lineptr, &linenum, arg->file_ptr);

    // while NOT at the end of file...
    // ... read each line and process
    while(num_chars_inline >= 0) {
        // grab first word from line
        word = strtok(lineptr, " ");

        // now execute command based on command
        while(word != NULL) {
            // if the command is proc
            if(strcmp(word, "proc") == 0) {
                // create PCB structure
                curr_struct = createStruct();
            }


            // if the command is sleep
                // sleep the given ms

            // if the command is stop
                // break
        }

    }
    return 0;
}

struct PCB *createStruct() {
    struct PCB *pcb = malloc(sizeof(struct PCB));
    pcb->next = NULL;
    pcb-> prev = NULL;
    return pcb;
}