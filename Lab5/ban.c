/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "banker.h"
#include <string.h>
#include <unistd.h>

// Put any other macros or constants here using #define

bank b;
int work[NUM_CUSTOMERS];
int finish[NUM_RESOURCES];

pthread_mutex_t mutex;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
pthread_mutex_t mutexAvail;
pthread_mutex_t mutexAlloc;
pthread_mutex_t mutexNeed;


// Define functions declared in banker.h here
void *thread_request_res(void *i_data)
{
	t_data* data = (t_data*) i_data;;
	//pthread_mutex_lock(&mutex3);
    /*printf("Responding to customer %d\n", data->n_customer);
	printf("Requested: ");
	for (int i = 0; i < NUM_RESOURCES; i++)
		printf("%d ", data->request[i]);
	printf("\n");*/
	if (isSafe())  {
        //printf("The system is in a safe state.\n");
		if (request_res(data->n_customer, data->request) == true) {
        	printf("Responding to customer %d\n", data->n_customer);
            printf("Requested: ");
            for (int i = 0; i < NUM_RESOURCES; i++)
                printf("%d ", data->request[i]);
            printf("\n");
            printf("Responding to customer %d\n", data->n_customer);
			printf("Accpeted. The resources are granted.\n");
		} else {
			//printf("Denied. The resources are not granted.\n");
			//sleep(rand() % 10);
		}
	}
    else
    {
       // printf("The system is in a unsafe state.\n");
    }

    // check release resource is all 0s.
    bool release_check = false;
    for (int i = 0; i < NUM_RESOURCES; i++)  {
        if (data->release[i] != 0)
            release_check = true;
    }

    if (release_check) {
        if (release_res(data->n_customer, data->release))  {
            /*printf("Resources released: ");
            for (int i = 0; i < NUM_RESOURCES; i++)
                printf("%d ", data->release[i]);
            printf("\n");*/
        } else {
            /*printf("Cannot release: ");
            for (int i = 0; i < NUM_RESOURCES; i++)
              printf("%d ", data->release[i]);
            printf("\n");*/
        }
    }
   // pthread_mutex_unlock(&mutex3);
    //printf("------------------------------------------------\n");
	
	return NULL;
}

bool request_res(int n_customer, int request[])
{
	// step 1
	for (int j = 0; j < NUM_RESOURCES; j++)  {
		if (request[j] > b.need[n_customer][j])
			return false;
	}

	for (int i = 0; i < NUM_RESOURCES; i++)  {
		if (request[i] > b.available[i]) {
			return false;
		}
	}



	for (int i = 0; i < NUM_RESOURCES; i++)  {

        pthread_mutex_lock(&mutexAvail);
		b.available[i] = b.available[i] - request[i];
        pthread_mutex_unlock(&mutexAvail);

        pthread_mutex_lock(&mutexAlloc);
		b.allocation[n_customer][i] = b.allocation[n_customer][i] + request[i];
		pthread_mutex_lock(&mutexAlloc);

        pthread_mutex_lock(&mutexNeed);
        b.need[n_customer][i] = b.need[n_customer][i] -  request[i];
	    pthread_mutex_unlock(&mutexNeed);
    }

	return true;
}

// Release resources, returns true if successful
bool release_res(int n_customer, int release[])
{
    for (int j = 0; j < NUM_RESOURCES; j++)  {
        if (release[j] > b.allocation[n_customer][j])
            return false;
    }

    for (int i = 0; i < NUM_RESOURCES; i++)  {
        if ((release[i] + b.need[n_customer][i]) > b.maximum[n_customer][i])
            return false;
    }

    for (int i = 0; i < NUM_RESOURCES; i++)  {

        pthread_mutex_lock(&mutexAvail);
        b.available[i] = b.available[i] + release[i];
        pthread_mutex_unlock(&mutexAvail);

        pthread_mutex_lock(&mutexAlloc);
        b.allocation[n_customer][i] = b.allocation[n_customer][i] - release[i];
        pthread_mutex_lock(&mutexAlloc);

        pthread_mutex_lock(&mutexNeed);
        b.need[n_customer][i] = b.need[n_customer][i] + release[i];
        pthread_mutex_unlock(&mutexNeed);

    }

    return true;;
}

bool isSafe()
{
    for (int i = 0; i < NUM_RESOURCES; i++)  {
    	finish[i] = false;
    }

	memcpy(work, b.available, sizeof(work));

    for (int i = 0; i < NUM_CUSTOMERS; i++)  {
    	if (finish[i] == false) {
    		for (int j = 0; j < NUM_RESOURCES; j++)  {
    			if (b.need[i][j] <= work[j])  {
    				for (int j = 0; j < NUM_RESOURCES; j++)  {
    					work[j] += b.allocation[i][j];
    				}
    				finish[i]= true;
    			}
    		}
    	}
    }

    for (int i = 0; i < NUM_RESOURCES; i++)  {
    	if (finish[i] == false)
    		return false;
    }
    return true;

}

int main(int argc, char *argv[])
{
    // ==================== YOUR CODE HERE ==================== //

    // Read in arguments from CLI, NUM_RESOURCES is the number of arguments   
	if (argc == NUM_RESOURCES + 1) {} 
	else	printf("Wrong resource types...\n");

    // Allocate the available resources
	for (int i  =0 ; i < NUM_RESOURCES; i++)
		b.available[i] = atoi(argv[i+1]);

	printf("Enter maximum resource table\n");
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        for(int j = 0; j < NUM_RESOURCES; j++) {
            scanf("%d", &b.maximum[i][j]);
        }
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        for(int j = 0; j < NUM_RESOURCES; j++) {
           b.allocation[i][j] = 0;
           b.need[i][j] = b.maximum[i][j];
        }
    }

    // Initialize the pthreads, locks, mutexes, etc.
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexAvail, NULL);
    pthread_mutex_init(&mutexAlloc, NULL);
    pthread_mutex_init(&mutexNeed, NULL);
    // Run the threads and continually loop
   	pthread_t threads[NUM_CUSTOMERS];
   	while (true)  {
   		for (int i = 0 ; i < NUM_CUSTOMERS; i++)  {
   		int request[NUM_RESOURCES];
        int release[NUM_RESOURCES];
   		
        bool request_check = false;
        while (request_check == false) {
            for (int i = 0; i < NUM_RESOURCES; i++)
       			request[i] = rand() % 10;
            
            // check request resource is all 0s.
            for (int i = 0; i < NUM_RESOURCES; i++)  {
                if (request[i] != 0)
                    request_check = true;
            }
        }

        for (int i = 0; i < NUM_RESOURCES; i++)
                release[i] = rand() % 10;
		t_data data;
		data.n_customer = i;
		memcpy(data.request, request, sizeof(request));
        memcpy(data.release, release, sizeof(release));
		pthread_create(&threads[i], 0, thread_request_res, &data);
		}
   	}

    for (int i = 0; i < NUM_CUSTOMERS; i++)  {
    	pthread_join(threads[i], 0);
    }


    // If your program hangs you may have a deadlock, otherwise you *may* have
    // implemented the banker's algorithm correctly
    
    // If you are having issues try and limit the number of threads (NUM_CUSTOMERS)
    // to just 2 and focus on getting the multithreading working for just two threads

    return EXIT_SUCCESS;
}
