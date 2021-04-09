#include "inc.h"

// create an initialized PCB node
struct PCB *createPCB(int ID) {
    struct PCB *pcb = malloc(sizeof(struct PCB));
    pcb->ID = ID;
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
    if(*head != NULL)
        printf("{ ID: %i, TotalBurstTime: %i }", (*head)->ID, (*head)->totalBurstTime);
    else
        printNULL();

    struct PCB *temp = *head;
    while(temp != NULL) {
        temp = temp -> next;
        printf(" --> ");
        if(temp != NULL) {
            printf("{ ID: %i, TotalBurstTime: %i }", temp->ID, temp->totalBurstTime);
        }
        else
            printNULL();
    }
    printf("\n");
    return;
}

void printNULL() {
    printf("{ NULL }");
}

// find the PCB node that contains smallest totalBurstTime
struct PCB *popSJF(struct PCB **head){
    // check if only one item in the list, if so, just pop
    if((*head)->next == NULL)
        return popQ(head);
    
    struct PCB *temp = *head;           // use temp to traverse list
    struct PCB *curr_shortest = temp;   // use curr_shortest to keep track of shortest burst time to return

    // find shortest burst time in the list
    temp = temp -> next;
    while(temp != NULL) {
        if(temp->totalBurstTime < curr_shortest->totalBurstTime)
            curr_shortest = temp;
        temp = temp -> next;
    }
    // remove the node from list
    if(curr_shortest->prev != NULL)
        curr_shortest->prev->next = curr_shortest->next;
    else
        *head = curr_shortest->next;
    if(curr_shortest->next != NULL)
        curr_shortest->next->prev = curr_shortest->prev;

    return curr_shortest;
}