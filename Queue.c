#include<stdio.h>
#include<stdlib.h>

#define CAPACITY    10

typedef struct {
    int* structBuffer;
} Queue;

int main(int argc, char **argv) {
    int buffer[CAPACITY];
    int* buffer2;
    buffer[0] = 6;
    //buffer2 = buffer;

    //printf("Before the queue's are made the og buffer's 0 element is %i.\n", buffer2[0]);

    Queue* q1;
    //q1->structBuffer = (int*) malloc(CAPACITY * sizeof(int));
    q1 = malloc(sizeof(Queue*));
    q1->structBuffer = buffer;

    printf("q1's buffer element 0 is %i.\n", q1->structBuffer[0]);

    Queue* q2 = (Queue*)malloc(sizeof(Queue));
    q2->structBuffer = buffer;

    printf("q2's buffer element 0 is %i.\n", q2->structBuffer[0]);

    free(q1);
    free(q2);
}