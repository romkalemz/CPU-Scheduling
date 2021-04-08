#include "inc.h"

// create an initialized PCB node
struct PCB *createPCB() {
    struct PCB *pcb = malloc(sizeof(struct PCB));
    pcb->next = NULL;
    pcb-> prev = NULL;
    return pcb;
};