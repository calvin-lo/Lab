/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"
#include "hostd.h"
#include <string.h>
#include <ctype.h>

// Put macros or constants here using #define
#define MEMORY 1024
#define RESERVED_MEMORY 64

// Put global environment variables here

int total_mem = MEMORY - RESERVED_MEMORY;


node_t *dispatch_list;
node_t *priority1;
node_t *priority2;
node_t *priority3;
node_t *user_jobs;
node_t *realtime;
int dispatcher_time = 0;
node_t *active = NULL;
int status = 0;
char *processArgv[] = {"./process", NULL}; 


// Define functions declared in hostd.h here
void print_process(proc process) 
{
    printf("-------------------------------------------------------------------------------------\n");
    printf("PID: %d\tPRIORITY: %d\tTIME: %d\t\tMEMORY: %d\tSIZE: %d\n", 
        process.pid, process.priority, process.processor_time, process.memory_index, process.MBytes);
    printf("#PRINTERS: %d\t#SCANNERS: %d\tMODEMS: %d\tCDs: %d\n",
        process.res.num_printers, process.res.num_scanners, process.res.num_modems, process.res.num_CDs);
    printf("-------------------------------------------------------------------------------------\n");

}

void load_dispatch(char *dispatch_file)
{
    FILE * file = fopen(dispatch_file, "r");
    while (!feof(file))  {
        int size = 8;
        int *info = malloc(sizeof(int) * size);
        int i = 0;
        while(!feof(file) && i < size)  {
            char number[] = "\0\0\0\0";
            char d = '\0';
            int n = 0;
            while (!feof(file) && isdigit(d = fgetc(file)))  {
                number[n++] = d;
            }
            if (strlen(number) > 0)  {
                info[i++] = atoi(number);
            }
            if(d == '\n')  {
                break;
            }
        }

        if (i < size)  {
            free(info);
            info = NULL;
        }

        proc process;
        process.pid = 0;
        process.arrival_time = info[0];
        process.priority = info[1];
        process.processor_time = info[2];
        process.MBytes = info[3];
        process.suspended = false;
        process.res.num_printers = info[4];
        process.res.num_scanners = info[5];
        process.res.num_modems = info[6];
        process.res.num_CDs = info[7];

        dispatch_list = push(dispatch_list, process);
    }
}

void add_queue(proc process)
{
    if (process.priority == 0)
        realtime = push(realtime, process);
    else if (process.priority == 1)
        priority1 = push(priority1, process);
    else if (process.priority == 2)
        priority2 = push(priority2, process);
    else if (process.priority == 3)
        priority3 = push(priority3, process); 
}

int main(int argc, char *argv[])
{
    // ==================== YOUR CODE HERE ==================== //
    
    // Load the dispatchlist
    char * dispatch_file = NULL;

    if(argc > 1)
        dispatch_file = argv[1];


    // Add each process structure instance to the job dispatch list queue
    load_dispatch(dispatch_file);

    // Iterate through each item in the job dispatch list, add each process
    // to the appropriate queues
    dispatcher_time = 0;
    
    do {
        init_res_avail();
        node_t *poped;
        while ( dispatch_list != NULL  && seek(dispatch_list)->process.arrival_time <= dispatcher_time)  {
            if (dispatch_list->next != NULL)  {
                poped = pop(dispatch_list);
                //printf("Time: %d\tPoped: %d\n", dispatcher_time, poped->process.arrival_time);
            }
            else {
                poped = dispatch_list;
                dispatch_list = NULL;
                //printf("Time: %d\tPoped: %d\n", dispatcher_time, poped->process.arrival_time);
            }

            // Check constrains and add to appro. queue
            if (poped->process.arrival_time <= dispatcher_time)  {
               if (poped->process.priority == 0)  {
                    if (poped->process.MBytes > RESERVED_MEMORY)  {
                        printf("Error: Max 64MB for real time job!\n");
                    } else if (poped->process.res.num_printers > 0 ||
                                poped->process.res.num_scanners > 0 ||
                                poped->process.res.num_modems > 0 ||
                                poped->process.res.num_CDs > 0) {
                        printf("Error: Real time job cannot include any resources!\n");
                    }  else {
                        realtime = push(realtime, poped->process);
                        //printf("pushed: %d\n", poped->process.arrival_time);
                    }
                } else {
                    if (poped->process.MBytes > total_mem)  {
                        printf("Error: Not enough memory!\n");
                    } else if (poped->process.res.num_printers > NUM_PRINTERS ||
                                poped->process.res.num_scanners > NUM_SCANNERS ||
                                poped->process.res.num_modems > NUM_MODEMS ||
                                poped->process.res.num_CDs > NUM_CDS) {
                        printf("Error: Not enough resources!\n");
                    }  else {
                        user_jobs = push(user_jobs, poped->process); 
                        //printf("pushed: %d\n", poped->process.arrival_time);                  
                    }
                }
            }
        }
        // Allocate the resources for each process before it's executed
        while(user_jobs != NULL && check_mem(user_jobs->process.res, user_jobs->process.MBytes) &&
                check_res(user_jobs->process.res))  {
                user_jobs->process.memory_index = alloc_mem(user_jobs->process.res, user_jobs->process.MBytes);
                node_t *temp;
                while( user_jobs != NULL)  {
                if (user_jobs->next != NULL)
                    temp = pop(user_jobs);
                else {
                    temp = user_jobs;
                    user_jobs = NULL;
                }
                alloc_res(temp->process);
                add_queue(temp->process);
            }
        }

    // Execute the process binary using fork and exec
       if (active != NULL) {
            active->process.processor_time--;
            if (active->process.processor_time == 0)  {
                kill(active->process.pid, SIGINT);
                waitpid(active->process.pid, &status, WUNTRACED);
                free_mem(active->process.res, active->process.memory_index, active->process.MBytes);
                free_res(active->process.res);
                free(active);
                active = NULL;
            } else if (active->process.priority > 0 && !(realtime == NULL && priority1 == NULL && priority2 == NULL && priority3 == NULL)) {
                kill(active->process.pid , SIGTSTP);
                waitpid(active->process.pid + 1, &status, WUNTRACED);
                active->process.suspended = true;
                if(active->process.priority < 3)  {
                    active->process.priority++;
                }
                add_queue(active->process); // add back to the queue
                active = NULL;
                
            }
       }

       // Pop process by priority.
       if (active == NULL && !(realtime == NULL && priority1 == NULL && priority2 == NULL && priority3 == NULL))  {
            if (realtime != NULL)  {
                if (realtime->next != NULL) {
                    active = pop(realtime);
                } else {
                    active = realtime;
                    realtime = NULL;
                }
            } else if (priority1 != NULL) {
                if (priority1->next != NULL) {
                    active = pop(priority1);
                } else {
                    active = priority1;
                    priority1 = NULL;
                }
            } else if (priority2 != NULL) {
                if (priority2->next != NULL) {
                    active = pop(priority2);
                } else {
                    active = priority2;
                    priority2 = NULL;
                }
            } else if (priority3 != NULL) {
                if (priority3->next != NULL) {;
                    active = pop(priority3);
                } else {
                    active = priority3;
                    priority3 = NULL;
                }
            }

            if(active->process.suspended == true)  {
                kill(active->process.pid, SIGCONT);
                active->process.suspended = false;
            } else {
                pid_t pid;
                pid = fork();
                if (pid < 0) {
                    printf("Error...\n");
                }
                else if (pid == 0)  {
                // child process   
                    execvp(processArgv[0], processArgv);
                }
                active->process.pid = pid;
                print_process(active->process);
                
            }        
        }
    sleep(1);
    dispatcher_time ++;
    //printf("Time: %d\n", dispatcher_time);
   } while (active != NULL || !(realtime == NULL && priority1 == NULL && priority2 == NULL && priority3 == NULL) || dispatch_list != NULL);
               

    // Perform the appropriate signal handling / resource allocation and de-alloaction

    // Repeat until all processes have been executed, all queues are empty
    
    return EXIT_SUCCESS;
}
