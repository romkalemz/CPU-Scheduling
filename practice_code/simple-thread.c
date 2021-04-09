#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
/* Simple program of thread structure for cpu-scheduler. */
/* compile with -pthread and -lrt */
struct PCB
{
    int count;
    int index;
    int nums[5];
} pcb;
sem_t sem_read, sem_cpu, sem_io;
struct timespec ts;
int file_read_done, cpu_sch_done;

void *readfile()
{
    int s;
    printf("In readFile thread\n");
    // get system clock set it to ts var
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    // add one second to curr time
    ts.tv_sec += 1;

    printf("readfile() about to call sem_timedwait()\n");
    // wait until sem_read is avaible or 1 second passes (should be since sem_read is init. with 1 )
    while ((s = sem_timedwait(&sem_read, &ts)) == -1 && errno == EINTR)
        continue; /* Restart if interrupted by handler */
    if (s == -1)
    {   
        // if s=-1 and errno=etimeout, means no lock avaible until time expired.
        // still locks after time expires to prevent deadlock
        if (errno == ETIMEDOUT)
            printf("sem_timedwait() timed out\n");
        else
            perror("sem_timedwait");
    }
    else
        printf("sem_timedwait() succeeded\n");
    // just initialize array
    for (int i = 0; i < 5; i++)
    {
        pcb.nums[i] = i;
    }
    // increment sem_cpu, allows cpuSchedule to proceed
    if (sem_post(&sem_cpu) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    file_read_done = 1;
    return NULL;
}
// cpuSchedule and ioSchedule same idea as readfile
void *cpuSchedule()
{
    int s;
    printf("In cpuSchedule thread\n");
    while (1)
    {
        if (file_read_done == 1 && pcb.index == pcb.count)
        {
            cpu_sch_done = 1;
            break;
        }
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        ts.tv_sec += 1;

        printf("cpuSchedule() about to call sem_timedwait()\n");
        while ((s = sem_timedwait(&sem_cpu, &ts)) == -1 && errno == EINTR)
            continue; /* Restart if interrupted by handler */
        if (s == -1)
        {
            if (errno == ETIMEDOUT)
                printf("sem_timedwait() timed out\n");
            else
                perror("sem_timedwait");
        }
        else
            printf("sem_timedwait() succeeded\n");
        printf("cpuNum = %d\n", pcb.nums[pcb.index]);
        pcb.index++;
        if (pcb.index == pcb.count)
        {
            cpu_sch_done = 1;
            if (sem_post(&sem_io) == -1)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            break;
        }
        if (sem_post(&sem_io) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

void *ioSchedule()
{
    int s;
    printf("In ioSchedule()");
    while (1)
    {
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        ts.tv_sec += 1;

        printf("ioSchedule() about to call sem_timedwait()\n");
        while ((s = sem_timedwait(&sem_io, &ts)) == -1 && errno == EINTR)
            continue; /* Restart if interrupted by handler */
        if (s == -1)
        {
            if (errno == ETIMEDOUT)
                printf("sem_timedwait() timed out\n");
            else
                perror("sem_timedwait");
        }
        else
            printf("sem_timedwait() succeeded\n");
        if (cpu_sch_done == 1 && file_read_done == 1)
        {
            break;
        }
        printf("ioNum: %d\n", pcb.nums[pcb.index]);
        pcb.index++;
        if (sem_post(&sem_cpu) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}
int main()
{
    int r;
    file_read_done = 0;
    cpu_sch_done = 0;
    pcb.count = 5;
    pcb.index = 0;
    // initialize the sema locks for read/cpu/io threads
    if (sem_init(&sem_read, 0, 1) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_init(&sem_cpu, 0, 0) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_init(&sem_io, 0, 0) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    // create 3 threads for
    pthread_t thread_id[3];
    if ((r = pthread_create(&thread_id[0], NULL, readfile, NULL)) != 0)
    {
        fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
        return EXIT_FAILURE;
    }
    if ((r = pthread_create(&thread_id[1], NULL, cpuSchedule, NULL)) != 0)
    {
        fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
        return EXIT_FAILURE;
    }
    if ((r = pthread_create(&thread_id[2], NULL, ioSchedule, NULL)) != 0)
    {
        fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
        return EXIT_FAILURE;
    }
    // join all threads/terminate
    for (int i = 0; i < 3; i++)
    {
        if ((r = pthread_join(thread_id[i], NULL)) == -1)
        {
            fprintf(stderr, "Error = %d (%s)\n", r, strerror(r));
            return EXIT_FAILURE;
        }
    }
    // destroy the semaphores
    if (sem_destroy(&sem_read) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_destroy(&sem_cpu) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (sem_destroy(&sem_io) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    printf("end of main\n");
    return 0;
}