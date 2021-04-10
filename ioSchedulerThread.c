#include "inc.h"
// IO_scheduler_thread
// Emulate by sleeping for given IO burst time, then release, and place in Ready_Q

void *ioSchedule()
{

    int s;
    struct timespec ts;

    while (1)
    {
        if (sem_post(&sem_mutex) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (file_read_done == 1 && cpuBusy == 0 && isEmptyQ(ready_q_head) == 0 && isEmptyQ(io_q_head) == 0)
        {
            ioBusy = 0;
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
        while ((s = sem_timedwait(&sem_io, &ts)) == -1 && errno == EINTR)
            continue; /* Restart if interrupted by handler */
        if (s == -1)
        {
            if (errno == ETIMEDOUT)
            {
                printf("[IO]   sem_timedwait() timed out\n");
            }
            else
            {
                perror("[IO]   sem_timedwait");
                exit(EXIT_FAILURE);
            }
        }

        ioBusy = 1;
        if (isEmptyQ(io_q_head) == 1)
        {
            struct PCB *temp = popQ(&io_q_head);
            // don't sleep if RR
            if(isAlgRR == 0){
               usleep(temp->IOBurst[temp->ioIndex]);
            }
            temp->ioIndex++;
            push(&ready_q_head, temp);
            //printf("        [AFTER] ");  printQ(&ready_q_head);
            if (sem_post(&sem_cpu) == -1)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        ioBusy = 0;
        // increment sem_cpu, allows cpuSchedule to proceed
    }
    return NULL;
}