/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "queue.h"

// Define your utility functions here, you will likely need to add more...
resources res_avail;


int rem_memory = MEMORY - RES_MEMORY;

int curr_index = 0;

void init_res_avail() {
    res_avail.num_printers = NUM_PRINTERS;
    res_avail.num_scanners = NUM_SCANNERS;
    res_avail.num_modems = NUM_MODEMS;
    res_avail.num_CDs = NUM_CDS;
    for (int i = 0 ; i < MEMORY ; i++)
        res_avail.memory[i] = 0;
}

int alloc_mem(resources res, int size)
{
    int start_index = -1;
    int i = 0;;
    for (i = curr_index; i < curr_index + size; i++)  {
        if (res_avail.memory[i] == 0 && start_index == -1)  {
            res_avail.memory[i] = 1;
            start_index = i;
        } else if (res.memory[i] == 0)  
            res_avail.memory[i] = 1;
    }
    curr_index = i;
    return start_index;

}

void free_mem(resources res, int index, int size)
{
    for (int i = index; i < index + size ; i++)  {
        res_avail.memory[i] = 0;
    }
    curr_index = index;

}

bool isEmpty(resources res)
{
    for (int i = 0; i < MEMORY; i++)  {
        if (res.memory[i] == 1)
            return false;
    }
    return true;
}

bool check_mem(resources res, int size)  
{
    int free_space = 0;
    for (int i = 0; i < MEMORY - RES_MEMORY; i++)  {
        if (res_avail.memory[i] == 0)  {
            free_space++;
        }
    }
    if (size < free_space)
        return true;
    else 
        return false;
}

bool check_res(resources res) 
{
    return (res.num_printers <= res_avail.num_printers && 
            res.num_scanners <= res_avail.num_scanners &&
            res.num_modems <= res_avail.num_modems && 
            res.num_CDs <= res_avail.num_CDs);
}

void free_res(resources res)  
{
    res_avail.num_printers += res.num_printers;
    res_avail.num_scanners += res.num_scanners;
    res_avail.num_modems += res.num_modems;
    res_avail.num_CDs += res.num_CDs;

    res.num_printers = 0;
    res.num_scanners = 0;
    res.num_modems = 0;
    res.num_CDs = 0;

}

bool alloc_res(proc process)  {
    int temp_printers = process.res.num_printers;
    int temp_scanners = process.res.num_scanners;
    int temp_modems = process.res.num_modems;
    int temp_CDs = process.res.num_CDs;

    if (check_res(process.res))
        return false;

    res_avail.num_printers -= temp_printers;
    res_avail.num_scanners -= temp_scanners;
    res_avail.num_modems -= temp_modems;
    res_avail.num_CDs -= temp_CDs;

    return true;

}



