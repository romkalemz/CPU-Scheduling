#include "inc.h"
// CPU_scheduler_thread
// Emulate by sleeping for given CPU burst time, then release, and place in IO_Q
void *cpuSchedule(void *args)
{
    struct ARG *arg = args;
    int s;
    int isRR = 0;
    int doneRR = 0;
    struct timespec ts;
    while (1)
    {
        if (file_read_done == 1 && !ioBusy && !isEmptyQ(ready_q_head) && !isEmptyQ(io_q_head))
        {
            cpu_sch_done = 1;
            cpuBusy = 0;
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

        if (sem_wait(&sem_mutex) == -1)
        {
            perror("sem_wait: mutex_sem");
            exit(1);
        }
        struct PCB *temp;
        cpuBusy = 1;

        // check what algorithm we are using
        if (strcmp(arg->algo, "FIFO") == 0)
            temp = popQ(&ready_q_head);

        else if (strcmp(arg->algo, "SJF") == 0)
            temp = popSJF_or_popPR(&ready_q_head, 0);

        else if (strcmp(arg->algo, "RR") == 0){
            isRR = 1;
            temp = popQ(&ready_q_head);
            if(temp -> cpuTime > temp -> quantumTime){
                usleep(temp -> quantumTime);
                temp -> cpuTime -= temp -> quantumTime;
            } else {
                usleep(temp -> cpuTime);
                temp -> cpuTime = 0;
                doneRR = 1;
            }
        }
        else if (strcmp(arg->algo, "PR") == 0)
        {
            temp = popSJF_or_popPR(&ready_q_head, 1);
        }
        if(isRR == 0){
            usleep(temp->CPUBurst[temp->cpuIndex]);
            temp->cpuIndex++;
        }
        if (temp->cpuIndex == temp->numCPUBursts || doneRR == 1)
        {
            // record necessary system clock times for report
            clock_gettime(CLOCK_MONOTONIC, &temp->ts_end);
            elapsed = temp->ts_end.tv_sec - temp->ts_begin.tv_sec;
            elapsed += (temp->ts_end.tv_nsec - temp->ts_begin.tv_nsec) / 1000000000.0;
            total_num_processes += 1;
            total_turnaround_time += (elapsed * 1000);
            total_waiting_time += (elapsed * 1000) - ((float)temp->CPUBurst[temp->cpuIndex] / 1000);
            cpuBusy = 0;
            free(temp);
        }
        else
        {
            temp->next = NULL;
            push(&io_q_head, temp);
            cpuBusy = 0;
        }
        // increment sem_io, allows ioSchedule to proceed
        if (sem_post(&sem_io) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (sem_post(&sem_read) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}