#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "queue.h"

/*
	Function: enqueue
	Description: 
		- creates a new node and adds it to the back of the linked list
	Parameters: 
		- head: the head of the linked list
		- id: the user_id of a customer
*/
Node *enqueue(Node *head, int id)
{

	Node *new_node = (Node *)malloc(sizeof(Node));
	new_node->id = id;
	new_node->next = NULL;

	if (head == NULL)
	{
		head = new_node;
	}
	else
	{
		Node *temp = head;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = new_node;
	}

	return head;
}

/*
	Function: dequeue
	Description: 
		- removes the node at the head of the linked list
		- returns the new head of the linked list
	Parameters: 
		- head: the head of the linked list
*/
Node *dequeue(Node *head)
{
	Node *temp = head;
	head = head->next;
	temp->next = NULL;
	free(temp);
	return head;
}

/*
	Function: isEmpty
	Description: 
		- returns true if the linked list is empty
		- returns false if the linked list has nodes
	Parameters: 
		- head: the head of the linked list
*/
bool isEmpty(Node *head)
{
	bool empty;
	if (head == NULL)
	{
		empty = true;
	}
	else
	{
		empty = false;
	}
	return empty;
}

/*
	Function: getLength
	Description: 
		- counts the number of nodes in a linked list
	Parameters: 
		- head: the head of the linked list
*/
int getLength(Node *head)
{
	int i = 0;
	Node *temp = head;
	while (temp != NULL)
	{
		i++;
		temp = temp->next;
	}
	return i;
}
