#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// Queue structure
typedef struct {
	int* data;
	int head, tail, size, capacity;
} Queue;

// thread parameters
typedef struct {
	Queue* buffer;
	int itemsManaged, sleeping, threadID;
} threadData;

// global variables
#define NUM_THREADS	4
#define CAPACITY	5
pthread_mutex_t mutex;
pthread_cond_t condProd, condCons;
threadData threadDataArr[NUM_THREADS];

/***
* Receives a Queue struct pointer and returns a boolean
* value of whether or not the Queue is full.
***/
int full(Queue* q) {return q->size == q->capacity;}

/***
 * Receives a Queue struct pointer and return a boolean
 * value of whether or not the Queue is empty.
 ***/
int empty(Queue* q) {return q->size == 0;}

/***
 * Receives a Queue struct pointer and an item to add
 * to the Queue. Adds the value to the end of the 
 * Queue or returns if full. 
 ***/
void enqueue(Queue* q, int item) {
	if(q->capacity == q->size) {
		printf("Queue is full.");
		return;
	}else if(q->size == 0) {
		q->data[q->tail] = item;
		q->size++;
	} else {
		q->tail = (q->tail + 1) % q->capacity;
		q->data[q->tail] = item;
		q->size++;
	}
}

/***
 * Receives a Queue struct pointer and removes the first 
 * item in the Queue. 
 ***/
void dequeue(Queue* q) {
	if(q->size == 0) {
		printf("Queue is empty.");
		return;
	} 
	q->head = (q->head + 1) % q->capacity;
	q->size--;
}

/***
 * The function for the producer threads. Continually adds
 * items to the buffer, if there is no space in the buffer 
 * the producer thread is put to sleep. 
 ***/
void *producer(void *threadArgs) {
	threadData* prodData = (threadData*) threadArgs;
	int item = 1;

	// Producer loop, runs until 100 items are added to the buffer. 
	while(prodData->itemsManaged < 100) {
		/*** Enter Critical Region ***/
		pthread_mutex_lock(&mutex);
		while(full(prodData->buffer)) { 		// checks for buffer space
			prodData->sleeping++;				// sleep counter
			pthread_cond_wait(&condProd, &mutex);
		}
		enqueue(prodData->buffer, item);		// puts item in the buffer
		prodData->itemsManaged++;				// produced items counter
		pthread_cond_signal(&condCons);			// wakes up a consumer thread
		pthread_mutex_unlock(&mutex);
		/*** Exit Critical Region ***/
	}
	pthread_exit(NULL);
}

/***
 * The function for the consumer threads. Continually removes
 * items from the buffer, if the buffer is empty the consumer
 * thread is put to sleep. 
 ***/
void *consumer(void *threadArgs) {
	threadData* consData = (threadData*) threadArgs;
	int item = 1;

	// Consumer loop, runs until 100 items are consumed.
	while(consData->itemsManaged < 100) {
		/*** Enter Critical Region ***/
		pthread_mutex_lock(&mutex);
		while(empty(consData->buffer)) {			// check is buffer is empty
			consData->sleeping++;					// sleep counter
			pthread_cond_wait(&condCons, &mutex);
		}
		dequeue(consData->buffer);					// takes items from buffer
		consData->itemsManaged++;					// consumed items counter
		pthread_cond_signal(&condProd);				// wakes up the consumer thread
		pthread_mutex_unlock(&mutex);
		/*** Exit Critical Region ***/
	}
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	int i;

	// Creating the queue, initializing member variables
	Queue* buffer = (Queue*)malloc(sizeof(Queue));
	buffer->head = buffer->tail = buffer->size = 0;
	buffer->capacity = CAPACITY;	
	buffer->data = (int*)malloc(buffer->capacity * sizeof(int));

	// initializing mutex and condition variables
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condProd, NULL);
	pthread_cond_init(&condCons, NULL);

	// initializing thread attributes, setting to joinable
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	// set thread parameters
	for(i = 0; i < NUM_THREADS; i++) {
		threadDataArr[i].buffer = buffer;		// pass each thread the Queue
		threadDataArr[i].threadID = i + 1;
	}

	// Create all the threads, 2 consumers and 2 producers
	pthread_create(&threads[0], &attr, consumer, (void *)&threadDataArr[0]);
	pthread_create(&threads[1], &attr, consumer, (void *)&threadDataArr[1]);
	pthread_create(&threads[2], &attr, producer, (void *)&threadDataArr[2]);
	pthread_create(&threads[3], &attr, producer, (void *)&threadDataArr[3]); 
	
	// join all the threads
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("The buffer size is %i.\n", CAPACITY);

	// print the data from all 4 running threads
	for(i = 0; i < NUM_THREADS; i++) {
		if(i < 2) {
			printf("Consumer thread #%i managed %i items and slept %i times.\n", 
			i + 1, threadDataArr[i].itemsManaged, threadDataArr[i].sleeping);
		} else {
			printf("Producer thread #%i managed %i items and slept %i times.\n", 
			i - 1, threadDataArr[i].itemsManaged, threadDataArr[i].sleeping);
		}
	}

	// clean up
	free(buffer->data);
	free(buffer);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condProd);
	pthread_cond_destroy(&condCons);
	pthread_exit(NULL);
	return 0;
}