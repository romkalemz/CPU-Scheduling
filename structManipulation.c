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

void push(struct PCB **first, struct PCB *newPCB){
    if(*first == NULL){
        *first = newPCB;
        return;
    }
    struct PCB *temp = *first;
    while(temp -> next != NULL) {
        temp = temp -> next;
    }
    temp->next = newPCB;
    newPCB->next = NULL;
    newPCB->prev = temp;
}
// returns first in pcb and removes from q
struct PCB *popQ(struct PCB **first){
    struct PCB *temp = *first;
    *first = temp -> next;
    return temp;
}
// checks if empty (whenr ready_q or io_q = null)
int isEmptyQ(struct PCB *first){
    if(first == NULL){
        return 0;
    }
    return 1;
}

void printQ(struct PCB **head) {
    printf("%p", *head);
    struct PCB *temp = *head;
    while(temp != NULL) {
        temp = temp -> next;
        printf(" --> %p", temp);
    }
    printf("\n");
    return;
}