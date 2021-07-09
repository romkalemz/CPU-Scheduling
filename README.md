# CPU-Scheduling
CS 460 Group Project 3
Spring Semester 2021

# CREATORS:
  Roman Stolyarov
  David Barko
  Daniel Yarmolenko
  

# DESCRIPTION:
  This program is a multi-threaded algorithm that measures the performance of the four basic
  CPU algorithms (FCFS, SJF, PR, RR) by simulating the processes of a CPU and an IO device
  that sleeps for the given corresponding burst-times by an input file.  


# COMPILE / RUN INSTRUCTIONS:
- to compile:
    "make"

- to run program:
    "./prog -alg [FIFO | SJF | PR | RR] [-quantum [integer (ms) ]] -input [file name]"


# FILES INCLUDED:
- main.c
    This file contains the main thread that creates the scheduler handling threads
    as well as the necessary variables and semaphores used in other files.

- fileReadThread.c
    This file deals with the parsement of the file and seperating the contents of the
    file into the proper variables and placed inside the designated structures.

- cpuSchedulerThread.c
    This file emulates and acts as the CPU scheduler which handles the processes passed
    to it, and processes them appropiately depending on the algorithm selected.
  
- ioSchedulerThread.c
    This file emulates and acts as the IO device and sleeps the given burst time listed
    on the input.txt file read into the program.

- structManipulation.c
    This file contains necessary functions that are used to manipulate the queues and structures
    so that accessing data, and moving it around is done efficiently.

- inc.h
    This file contains all of the include packages and files, function delcarations
    as well as necessary global variables that are used between all of the threads active.

- Makefile
    Makes compiling and running the program much more smoothly, cleaner, and faster.
