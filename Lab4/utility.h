/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_
 #include <stdbool.h>


// The amount of available memory
#define MEMORY 1024
#define RES_MEMORY 64
#define  NUM_PRINTERS 2  
#define  NUM_SCANNERS 1      
#define  NUM_MODEMS  1         
#define  NUM_CDS     2  


// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
  	int num_printers;
  	int num_scanners;
  	int num_modems;
  	int num_CDs;
 	  int memory[MEMORY];
} resources;

// Processes structure containing all of the process details parsed from the 
// input file, should also include the memory address (an index) which indicates
// where in the resources memory array its memory was allocated
typedef struct {
	   int pid;
  	int arrival_time;
  	int priority;
  	int processor_time;
  	int MBytes;
  	bool suspended;
  	int memory_index;
  	resources res;

} proc;


// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following examples:

// Function to allocate a contiguous chunk of memory in your resources structure
// memory array, always make sure you leave the last 64 values (64 MB) free, should
// return the index where the memory was allocated at
extern int alloc_mem(resources res, int size);

// Function to free the allocated contiguous chunk of memory in your resources
// structure memory array, should take the resource struct, start index, and 
// size (amount of memory allocated) as arguments
extern void free_mem(resources res, int index, int size);

extern bool isEmpty(resources res);

extern bool check_mem(resources res, int size);

extern bool check_res(resources res);

extern void free_res(resources res);

extern bool alloc_res(proc process);

extern void init_res_avail();




#endif /* UTILITY_H_ */