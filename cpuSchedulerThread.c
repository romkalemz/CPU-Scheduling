#include "inc.h"
// CPU_scheduler_thread
// Emulate by sleeping for given CPU burst time, then release, and place in IO_Q
void *cpuSchedule(void *args)
{
    struct ARG *arg = args;
    int s;
    struct timespec ts;
    while (1)
    {
        if(file_read_done == 1 && !ioBusy && !isEmptyQ(ready_q_head) && !isEmptyQ(io_q_head)){
           cpu_sch_done = 1;
            break;
        }
        // get system clock set it to ts var
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        // add one second to curr time
        ts.tv_sec += 1;

        //printf("[CPU]  cpuSchedule() about to call sem_timedwait()\n");
        // wait until sem_read is avaible or 1 second passes (should be since sem_read is init. with 1 )
        while ((s = sem_timedwait(&sem_cpu, &ts)) == -1 && errno == EINTR)
            continue; /* Restart if interrupted by handler */
        if (s == -1)
        {
            if (errno == ETIMEDOUT)
            {
                printf("[CPU]  sem_timedwait() timed out\n");
            }
            else
            {
                perror("[CPU]  sem_timedwait");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            //printf("[CPU]  cpu sem_timedwait() succeeded\n");
        }
        struct PCB *temp;
        cpuBusy = 1;
        // check what algorithm we are using
        //printf("    [attempting to pop from readyQ]\n");
        //printf("        [BEFORE] "); printQ(&ready_q_head);
        if(strcmp(arg->algo, "FIFO") == 0)
            temp = popQ(&ready_q_head);
        
        else if(strcmp(arg->algo, "SJF") == 0) 
            temp = popSJF_or_popPR(&ready_q_head, 0);
        
        else if(strcmp(arg->algo, "RR") == 0) 
            temp = popSJF_or_popPR(&ready_q_head, 1);
        
        else if(strcmp(arg->algo, "PR") == 0) {}
        
        //printf("        [AFTER]  ");  printQ(&ready_q_head);
        //printf("[CPU]  CPU burst for: %d\n", temp->CPUBurst[temp->cpuIndex]);
        usleep(temp->CPUBurst[temp->cpuIndex]);
        temp->cpuIndex++;

        if (temp->cpuIndex == temp->numCPUBursts)
        {
            // record necessary system clock times for report
            clock_gettime(CLOCK_MONOTONIC, &temp->ts_end);
            elapsed = temp->ts_end.tv_sec - temp->ts_begin.tv_sec;
            elapsed += (temp->ts_end.tv_nsec - temp->ts_begin.tv_nsec) / 1000000000.0;
            total_num_processes += 1;
            total_turnaround_time += (elapsed * 1000);
            total_waiting_time += (elapsed * 1000) - ((float) temp->CPUBurst[temp->cpuIndex] / 1000);

            cpuBusy = 0;
            free(temp);
        }
        else
        {
            temp->next = NULL;
            //printf("    [attempting to push into ioQ] %p\n", temp);
            //printf("        [BEFORE] "); printQ(&io_q_head);
            push(&io_q_head, temp);
            //printf("        [AFTER] ");  printQ(&io_q_head);
            cpuBusy = 0;
            // increment sem_io, allows ioSchedule to proceed
            if (sem_post(&sem_io) == -1)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    return NULL;
}