#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct Node Node;

struct Node
{
    int id;
    Node *next;
};

Node *enqueue(Node *head, int id);
Node *dequeue(Node *head);
bool isEmpty(Node *head);
int getLength(Node *head);

#endif