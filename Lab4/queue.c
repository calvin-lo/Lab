/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U:  Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file

node_t *push(node_t *tail, proc process)
{	
	node_t *new_node;
	new_node = malloc(sizeof(node_t));
	new_node->process = process;
	new_node->next = tail;
	tail = new_node;

	return tail;
}

node_t *pop(node_t *tail)
{
	node_t *curr = tail;
	if (curr->next != NULL) {
		node_t *prev = NULL;
		while(curr->next != NULL)  {
			if (curr->next->next == NULL)
				prev = curr;
			curr = curr->next;
		}
		prev->next = NULL;
	} else {
		//free(tail);
		//tail = NULL;
		curr = malloc(sizeof(struct node_t));
	}
	return curr;
}

node_t *seek(node_t *tail)  {
	node_t *curr = tail;
	while (curr->next != NULL) {
		if (curr->next != NULL)
			curr = curr->next;
	}
	return curr;
}
