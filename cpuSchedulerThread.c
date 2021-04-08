#include "inc.h"
// CPU_scheduler_thread
// Emulate by sleeping for given CPU burst time, then release, and place in IO_Q
void *cpuSchedule()
{
    int s;
    struct timespec ts;
    printf("In cpuSchedule thread\n");
    while (1)
    {
         // get system clock set it to ts var
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        // add one second to curr time
        ts.tv_sec += 1;

        printf("cpuSchedule() about to call sem_timedwait()\n");
        // wait until sem_read is avaible or 1 second passes (should be since sem_read is init. with 1 )
        while ((s = sem_timedwait(&sem_cpu, &ts)) == -1 && errno == EINTR)
            continue; /* Restart if interrupted by handler */
        if (s == -1)
        {
            if (errno == ETIMEDOUT)
            {
                printf("sem_timedwait() timed out\n");
            }
            else
            {
                perror("sem_timedwait");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("cpu sem_timedwait() succeeded\n");
        }
        cpuBusy = 1;
        struct PCB *temp = popQ(ready_q_head);
        printf("CPU burst for: %d\n", temp->CPUBurst[temp->cpuIndex]);
        usleep(temp->CPUBurst[temp->cpuIndex]);
        temp->cpuIndex++;

        if (temp->cpuIndex == temp->numCPUBursts)
        {
            cpu_sch_done = 1;
            break;
        }
        else
        {
            io_q_head = push(io_q_head, temp);
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