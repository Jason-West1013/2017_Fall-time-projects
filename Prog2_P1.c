#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

// Thread Variables
typedef struct {
    long threadID; 
    int notTurn;   // Not turn counter
} threadData;

// Global Variables
#define NUM_THREADS 4       // constant for the number of threads
pthread_mutex_t mutex;      // mutex lock 
pthread_cond_t cond_var;    // condition variable
int CurrentID;              // the shared variable 
threadData threadDataArr[NUM_THREADS];

/***
 * Primary thread function, receives a parameter
 * of the thread variables. Each thread takes 
 * turns accessing the shared variable CurrentID. 
`***/
void *takingTurns(void *threadArg) {
    threadData *myData = (threadData *) threadArg;
    int turn = 0;
    int i;

    // Main loop, each thread is terminated when they reach 10 turns. 
    while(turn < 10) {
        /*** Enter Critical Region ***/
        pthread_mutex_lock(&mutex);
        if(CurrentID == myData->threadID) {             // checks for turn 
            turn++;
            printf("My Turn! Thread #%ld\n", myData->threadID);
            CurrentID++;
            if(CurrentID == 5) {                        // resets CurrentID id value if 5
                CurrentID = 1;
            }
            pthread_cond_broadcast(&cond_var);          // wake up all sleeping threads
        } else {                                        // if not turn thread goes to sleep 
            printf("Not My Turn! Thread #%ld\n", myData->threadID);
            myData->notTurn++;
            pthread_cond_wait(&cond_var, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        /*** Exit Critcal Region ***/
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    CurrentID = 1;
    long i;

    // initializing mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);
    

    // Creating the threads
    for(i = 0; i < NUM_THREADS; i++) {
        printf("Creating thread number %ld\n", i);
        threadDataArr[i].threadID = i + 1;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&thread[i], &attr, takingTurns, (void *) &threadDataArr[i]);
    }

    // join all threads
    for(i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    // print out data
    for(i = 0; i < NUM_THREADS; i++) {
        printf("Thread #%ld made %i not Turns\n", i + 1, threadDataArr[i].notTurn);
    }

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex); 
    pthread_cond_destroy(&cond_var);
    pthread_exit(NULL); 
    return 0;
}