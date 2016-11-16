#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

//this link always point to first Link
struct node *head = NULL;

void display(struct node *node)
{
	while(node!=NULL){
		printf("%c", node->data);
		node = node->next;
	}
}
void insertFirst(char data)
{
	
}

/* the offset marks the position of the cursor from the starting of the head_ref
 we can set the head_ref as the starting position of a particular line or the 
 starting position of the entire document. The first positions will be inserted via insertFirst. */
void insertAfter(struct node** head_ref, int offset, char newData)
{
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	
	struct node* temp = (*head_ref);
}

void deleteFirst()
{
	
}

/* the offset marks the position of the cursor from the starting of the head_ref
 we can set the head_ref as the starting position of a particular line or the 
 starting position of the entire document */
void deleteAfter(struct node** head_ref, int offset)
{
	
}
