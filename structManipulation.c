#include "inc.h"

// create an initialized PCB node
struct PCB *createPCB() {
    struct PCB *pcb = malloc(sizeof(struct PCB));
    pcb->next = NULL;
    pcb-> prev = NULL;
    pcb-> cpuIndex = 0;
    pcb -> ioIndex = 0;
    return pcb;
};

struct PCB *push(struct PCB *first, struct PCB * newPCB){
    struct PCB *temp = first;
    if(first == NULL){
        return newPCB;
    }
    while(temp->next != NULL){
        temp = temp -> next;
       // printf("pushpcb function: %d\n", temp ->priority);
    }
    temp->next = newPCB;
    newPCB->prev = temp;

    return first;
}
// returns first in pcb and removes from q
struct PCB *popQ(struct PCB *first){
    struct PCB *temp;
    temp = first;
    first = temp -> next;
    return temp;
}
// checks if empty (whenr ready_q or io_q = null)
int isEmptyQ(struct PCB *first){
    if(first == NULL){
        return 0;
    }
    return 1;
}