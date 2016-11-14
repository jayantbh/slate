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

//this link always point to last Link 
struct node *last = NULL;

struct node *current = NULL;

//is list empty
bool isEmpty()
{
	return head == NULL;
}

int length()
{
	int length = 0;
	struct node *current;
	
	for(current = head; current != NULL; current = current->next){
		length++;
	}
	
	return length;
}

//display the list in from first to last
void display() 
{

    //start from the beginning
    struct node *ptr = head;
	
    //navigate till the end of the list
    while(ptr != NULL) {        
		printf("%c",ptr->data);
		ptr = ptr->next;
    }
	
}


//insert link at the first location
void insertFirst(int data) 
{

    //create a link
    struct node *link = (struct node*) malloc(sizeof(struct node));
    link->data = data;
	
    if(isEmpty()) {
       //make it the last link
       last = link;
    } else {
       //update first prev link
       head->prev = link;
    }

    //point it to old first link
    link->next = head;
	
    //point first to new first link
    head = link;
}

//insert link at the last location
void insertLast(int data) 
{

    //create a link
	struct node *link = (struct node*) malloc(sizeof(struct node));
    link->data = data;
	
    if(isEmpty()) {
       //make it the last link
       last = link;
    } else {
       //make link a new last link
       last->next = link;     
      
       //mark old last node as prev of new link
       link->prev = last;
    }

	//point last to new last node
	last = link;
}

//delete first item
struct node* deleteFirst() {

   //save reference to first link
   struct node *tempLink = head;
	
   //if only one link
   if(head->next == NULL){
      last = NULL;
   } else {
      head->next->prev = NULL;
   }
	
   head = head->next;
   //return the deleted link
   return tempLink;
}

//delete link at the last location

struct node* deleteLast() {
   //save reference to last link
   struct node *tempLink = last;
	
   //if only one link
   if(head->next == NULL) {
      head = NULL;
   } else {
      last->prev->next = NULL;
   }
	
   last = last->prev;
	
   //return the deleted link
   return tempLink;
}

//delete a link with given key

struct node* deleteAfter() 
{
}

bool insertAfter() 
{
}

main() 
{
}