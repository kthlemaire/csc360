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
#include "linked_list.h"

/*
	Function: add_newNode
	Description: 
		Creates a new node with a pid and a path. 
		Places the new node before the head. 
		Returns the new_node
	Parameters: 
		- head: the head of the linked list
		- new_pid: the pid of the new process
		- new_path: the path of the new process
*/
Node *add_newNode(Node *head, pid_t new_pid, char new_path[])
{
	Node *new_node = (Node *)malloc(sizeof(Node));
	new_node->pid = new_pid;
	strcpy(new_node->path, new_path);
	new_node->next = head;
	return new_node;
}

/*
	Function: deleteNode
	Description: 
		Finds the node with the given pid. 
		Deletes the node from the linked list. 
		Returns the head of the linked list. 
	Parameters: 
		- head: the head of the linked list
		- pid: the pid associated with the node we want to delete
*/
Node *deleteNode(Node *head, pid_t pid)
{
	Node *prev = NULL;
	Node *temp = head;
	while (temp != NULL)
	{
		if (temp->pid == pid)
		{
			if (temp == head)
			{
				head = temp->next;
				temp->next = NULL;
				free(temp);
				return head;
			}
			else
			{
				prev->next = temp->next;
				temp->next = NULL;
				free(temp);
				return head;
			}
		}
		prev = temp;
		temp = temp->next;
	}
	return head;
}

/*
	Function: printList
	Description: 
		Prints the pid followed by the path of the node
	Parameters: 
		- node: the node we want to print
*/
void printList(Node *node)
{
	printf("%d: %s\n", node->pid, node->path);
}

/*
	Function: PidExists
	Description: 
		Determines if the given pid is associated with a node in the linked list. 
	Parameters: 
		- head: the head of the linked list
		- pid: the pid of the process we are checking
*/
bool PidExist(Node *head, pid_t pid)
{
	Node *temp = head;
	while (temp != NULL)
	{
		if (temp->pid == pid)
		{
			return true;
		}
		temp = temp->next;
	}
	return false;
}
