#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

//this link always point to first Link

void display(struct node** head_ref)
{
	while(node!=NULL){
		printf("%c", node->data);
		node = node->next;
	}
}

//push a character at the beginning of the Linked List
void push(struct node** head_ref, char newData)
{
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	
	newNode->next = (*head_ref);
	newNode->prev = NULL;
		
	if((*head_ref)! = NULL)
		(*head_ref)->prev = newNode;
	
	(*head_ref) = newNode;
	
	return;
}

//insert a character after a given node
void insertAfter(struct node* prevNode, char newData)
{
	if(prevNode == NULL)
		return; //error has occurred
	
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	
	newNode->next = prevNode->next;
	prevNode->next = newNode;
	newNode->prev = prevNode;
	if(newNode->next != NULL)
		newNode->next->prev = newNode;
	
	return;
}

//append a character to the end of the Linked List
void append(struct node** head_ref, char newData)
{
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	newNode->next = NULL;
	
	struct node* last = (*head_ref);
	if((*head_ref) == NULL){
		newNode->prev = NULL;
		*head_ref = newNode;
		return;
	}
	while(last->next != NULL)
		last = last->next;
	
	last->next = newNode;
	newNode->prev = last;
	
	return;
}

//traverse the Linked List to the right, towards the tail
void goRight(struct node* currNode)
{
	if(currNode->next != NULL)
		currNode = currNode->next;
	return;
}

//traverse the Linked List to the left, towards the head
void goLeft(struct node* currNode)
{
	if(currNode->prev != NULL)
		currNode = currNode->prev;
	return;
}


void main()
{
	
}
