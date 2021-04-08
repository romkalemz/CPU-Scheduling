#include "inc.h"
// IO_scheduler_thread
// Emulate by sleeping for given IO burst time, then release, and place in Ready_Q

void *ioSchedule()
{

    int s;
    struct timespec ts;
    printf("In ioSchedule thread\n");
    while (1)
    {
        if(file_read_done == 1 && cpu_sch_done == 1 && isEmptyQ(ready_q_head) == 1 && isEmptyQ(io_q_head) == 1){
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

        printf("ioSchedule() about to call sem_timedwait()\n");
        // wait until sem_read is avaible or 1 second passes (should be since sem_read is init. with 1 )
        while ((s = sem_timedwait(&sem_io, &ts)) == -1 && errno == EINTR)
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
            printf("io sem_timedwait() succeeded\n");
        }

        struct PCB *temp = popQ(io_q_head);
        printf("IO burst for: %d\n", temp->IOBurst[temp->ioIndex]);
        usleep(temp->IOBurst[temp->ioIndex]);
        temp->ioIndex++;

        ready_q_head = push(ready_q_head, temp);
        // increment sem_cpu, allows cpuSchedule to proceed
        if (sem_post(&sem_cpu) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}