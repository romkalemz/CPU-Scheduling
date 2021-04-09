#include "inc.h"
// FileRead_thread
// Read the inputted file, create (emulated) process, place in Ready_Q
void *fileRead(void *args)
{
    struct ARG *arg = args;
    struct PCB *curr_pcb;
    // read first line
    bool proc_command = false;
    char *word, *lineptr = NULL;
    size_t linenum = 0;
    ssize_t num_chars_inline = getline(&lineptr, &linenum, arg->file_ptr);
    // while NOT at the end of file...
    // ... read each line and process
    while (num_chars_inline > 0)
    {
        // grab first word from line
        word = strtok(lineptr, " ");

        // now execute command based on word
        for (int i = -3; word != NULL; i++)
        {
            // i = -3 --> command keyword
            // i = -2 --> priority variable
            // i = -1 --> total number of bursts in the line
            // i >= 0 --> list of CPU / IO bursts

            // if the command is proc
            if (strcmp(word, "proc") == 0)
            {
                // be ready to push into ready_Q once proc command finishes (no more numbers in the line)
                proc_command = true;
                // create PCB structure
                curr_pcb = createPCB();
            }

            // if the command is sleep
            else if (strcmp(word, "sleep") == 0)
            {
                word = strtok(NULL, " ");
                // sleep the given ms
                usleep(atoi(word));
                break; // read next line
            }
            // if the command is stop
            else if (strcmp(word, "stop") == 0)
            {
                // free necessary vars
                free(lineptr);
                // close file
                fclose(arg->file_ptr);
                // return
                file_read_done = 1;
                return (void *)0;
            }
            // priority int intialization
            else if (i == -2)
                curr_pcb->priority = atoi(word); // update priority from the line section

            // this is the number of total bursts in the proc command
            else if (i == -1)
            {
                curr_pcb->totalBursts = atoi(word);
                curr_pcb->numCPUBursts = ceil((double)curr_pcb->totalBursts / 2);
                curr_pcb->numIOBursts = floor((double)curr_pcb->totalBursts / 2);
                // init CPU and IO arrays
                curr_pcb->IOBurst = malloc(sizeof(int) * curr_pcb->numCPUBursts);
                curr_pcb->CPUBurst = malloc(sizeof(int) * curr_pcb->numIOBursts);
            }
            // otherwise its the list of bursts
            else
            {
                if (i % 2 == 0) // even numbers are CPU
                    curr_pcb->CPUBurst[i / 2] = atoi(word);
                else // odd numbers are IO
                    curr_pcb->IOBurst[i / 2] = atoi(word);
            }

            word = strtok(NULL, " ");
        }
        // if we just finished a proc command, push into ready_Q
        // also unlocking CPU thread to do its thing
        if (proc_command == true)
        {
            proc_command = false;
            printf("[FILE] Unlocking CPU -- priority: %i   total bursts: %i\n", curr_pcb->priority, curr_pcb->totalBursts);
            printf("[FILE] CPU array: ");
            for (int i = 0; i < curr_pcb->numCPUBursts; i++)
                printf("%i ", curr_pcb->CPUBurst[i]);
            printf("\n");
            printf("[FILE] IO array : ");
            for (int i = 0; i < curr_pcb->numIOBursts; i++)
                printf("%i ", curr_pcb->IOBurst[i]);
            printf("\n");
            //push pcb into ready_q
            //printf("    [attempting to push to readyQ] %p\n", curr_pcb);
            //printf("        [BEFORE] "); printQ(&ready_q_head);
            clock_gettime(CLOCK_MONOTONIC, &curr_pcb->ts_begin);
            push(&ready_q_head, curr_pcb);
            //printf("        [AFTER] ");  printQ(&ready_q_head);
            // allow sem_cpu to proceed
            if (sem_post(&sem_cpu) == -1)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        // grab next line, and loop
        num_chars_inline = getline(&lineptr, &linenum, arg->file_ptr);
    }
    // if this point is reached, no stop command was found
    // return error
    return (void *)1;
}