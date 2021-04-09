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
        printf("{ ID: %i, TotalBurstTime: %i, Priority: %i }", (*head)->ID, (*head)->totalBurstTime, (*head)->priority);
    else
        printNULL();

    struct PCB *temp = *head;
    while(temp != NULL) {
        temp = temp -> next;
        printf(" --> ");
        if(temp != NULL) {
            printf("{ ID: %i, TotalBurstTime: %i, Priority: %i }", temp->ID, temp->totalBurstTime, temp->priority);
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
struct PCB *popSJF_or_popPR(struct PCB **head, int flag){
    // check if only one item in the list, if so, just pop
    if((*head)->next == NULL)
        return popQ(head);

    struct PCB *temp = *head;           // use temp to traverse list
    struct PCB *winner = temp;   // use curr_shortest to keep track of shortest burst time to return

    // traverse list comparing properties of processes
    temp = temp -> next;
    while(temp != NULL) {
        // this is SJF comparison
        if(flag == 0) {
            if(temp->totalBurstTime < winner->totalBurstTime)
                winner = temp;
        }
        // this is RR comparison
        else if(flag == 1) {
            if(temp->priority > winner->priority)
                winner = temp;
        }
        temp = temp -> next;
    }
    // remove the node from list
    if(winner->prev != NULL)
        winner->prev->next = winner->next;
    else
        *head = winner->next;
    if(winner->next != NULL)
        winner->next->prev = winner->prev;

    return winner;
}