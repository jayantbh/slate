#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_STACK_SIZE 100

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

struct undoNode{
	struct node* uNode;
	char op;
	int moves;
};

struct undoNode undo_stack[MAX_STACK_SIZE];
int undoCursor = -1;

int move = 0;

int undoPush(struct node* currNode, char operation, int newmoves);
struct undoNode undoPop(void);
struct node* undo(struct node* currNode);

struct node* getHead(struct node* currNode);
void displayAll(struct node* currNode);
void displayCurrN(struct node* currNode, int N);
struct node* goRight(struct node* currNode, int n);
struct node* goLeft(struct node* currNode, int n);
struct node* moveCursor(struct node* currNode, int n);
struct node* insertCharAfter(struct node* currNode, char newData);
struct node* deleteChar(struct node* currNode);
struct node* loadFileToList(char *filename);
void writeBackToFile(struct node* head_ref, char *filename);

// return the head of the LinkedList
struct node* getHead(struct node* currNode)
{
	struct node* temp = currNode;
	while(temp->prev!=NULL){
		temp = temp->prev;
	}
	return temp;
}

//display all characters
void displayAll(struct node* currNode)
{
	struct node* temp = getHead(currNode);
	//printf("%c",temp->data);
	while (temp != NULL)
    {
        printf("%c", temp->data);
        temp = temp->next;
    }
    return;
}

//display N characters from the current Node
void displayCurrN(struct node* currNode, int N)
{
	struct node* temp = (currNode);
	if(N<0){ //move left
		while(temp->prev!=NULL && N<0){
			printf("%c", temp->data);
			temp = temp->prev;
			N++;
		}
	}
	else{ //move right
		while (temp->next != NULL && N>0)
    	{
        	printf("%c", temp->data);
        	temp = temp->next;
        	N--;
    	}
	}
    return;
}

//traverse the Linked List to the right, towards the tail
struct node* goRight(struct node* currNode, int n)
{
	struct node* temp = currNode;
	while(temp->next != NULL && n>0){
		temp = temp->next;
		//printf("\n*****\n%c, %d\n",temp->data,n);
		n--;
	}
	return temp;
}

//traverse the Linked List to the left, towards the head
struct node* goLeft(struct node* currNode, int n)
{
	struct node* temp = currNode;
	while(temp->prev != NULL && n<0){
		temp = temp->prev;
		//printf("\n*****\n%c, %d\n",temp->data,n);
		n++;
	}
	return temp;
}

//API METHODS

struct node* moveCursor(struct node* currNode, int n)
{
	struct node* temp = currNode;
	if(n>0){
		temp = goRight(temp, n);
		move+=n;
	}
	else{
		temp = goLeft(temp, n);
		move+=n;
	}
	return temp;
}
//API for inserting a character after a given node
struct node* insertCharAfter(struct node* currNode, char newData)
{
	if(currNode->data == '\0'){
		struct node* newNode = (struct node*) malloc(sizeof(struct node));
		newNode->data = newData;
		newNode->prev= NULL;
		newNode->next= NULL;
		return newNode;
	} //head node created
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	newNode->next = currNode->next;
	currNode->next = newNode;
	newNode->prev = currNode;
	if(newNode->next != NULL) newNode->next->prev = newNode;
	currNode=moveCursor(currNode,1);
	
	undoPush(newNode,'I',move); // when a new character is inserted it is pushed onto the undo stack
	move = 0;
	return currNode;
}

//API for deleting a character at the current node
struct node* deleteChar(struct node* currNode)
{
	//if(currNode == NULL) return -1; //error has occurred
	struct node* prevNode = moveCursor(currNode,-1);
	if(currNode->prev!=NULL) 
		currNode->prev->next = currNode->next;
	else
		currNode->next->prev = NULL;
	if(currNode->next!=NULL) 
		currNode->next->prev = currNode->prev;
	else
		currNode->prev->next = NULL;
	undoPush(currNode,'D',move); // when a new character is deleted it is pushed onto the undo stack
	//free(currNode);
	move = 0;
	return prevNode;
}

/*
	Take the characters from the file and insert them into a string (fast loading).
	Now construct the linkedlist from the string.
	Return the head pointer of the Linked List.
*/
struct node* loadFileToList(char *filename)
{
	struct node* head = (struct node*) malloc(sizeof(struct node)); struct node* memory1;
	head->prev = head->next = NULL;
	char* list = malloc(100000); char ch;
	int n = 0, i=0;
	FILE *file;
	file = fopen(filename,"r+");
	if (!file){
		file = fopen(filename,"w+");
		if(!file){
			printf("File could not be opened. Press any key to continue! \n\a\a");
			getchar();
			return NULL;
		}
	}
	while((ch=fgetc(file))!=EOF){
		list[n++] = (char)ch;
	}
	fclose(file);
	list[n] = '\0';
	head->data = list[0];
	while(list[i]!='\0'){
		i++;
		memory1 = (struct node*)malloc(sizeof(struct node));
   		memory1->data = list[i];
   		memory1->prev = head;
   		memory1->next = NULL;
   		head->next = memory1;
	   	head = memory1;
	}
	printf("\nFile Loaded!\n");
	return getHead(head);
}

//write the linked list back into the file
void writeBackToFile(struct node* head_ref, char *filename)
{
	head_ref = getHead(head_ref);
	FILE *file = fopen(filename,"w");
	if (!file){		printf("File could not be opened. Press any key to continue! \n\a\a");		getchar();		return;	}
	while(head_ref != NULL){
		fputc(head_ref->data, file);
		head_ref = head_ref->next;
	}
	fclose(file);
}

int undoPush(struct node* currNode, char operation, int newmoves)
{
	undoCursor++;
	if(undoCursor>MAX_STACK_SIZE-1) return -1;
	undo_stack[undoCursor].uNode=currNode;
	undo_stack[undoCursor].op=operation;
	undo_stack[undoCursor].moves=newmoves;
	return 1;
}

struct undoNode undoPop(void)
{
	//if(undoCursor<0) return NULL;
	struct undoNode unNode = undo_stack[undoCursor];
	undoCursor--;
	return unNode;
}

struct node* undo(struct node* currNode)
{
    //printf("Cursor: %d\n",undoCursor);
	struct undoNode unNode = undoPop();
	//printf("Cursor: %d\n",undoCursor);
	//if(unNode == NULL) return -1;
	char oper = unNode.op;
	struct node* prevNode;
	//printf("%d\n",move);
	switch(oper){
		case 'I':
			 //return 1 for successful deletion and -1 for some error
			
			//printf("Deleting after making %d moves\n",move);
			move = -1 * unNode.moves;
			printf("Deleting after making %d moves\n",move);
			prevNode = moveCursor(prevNode,move);
			prevNode = deleteChar(currNode);
			undoPop();
			return prevNode;
		case 'D':
		     //return 1 for successful insertion and -1 for some error
		    
		    
		    //printf("Inserting after making %d moves\n",move);
			move = -1 * unNode.moves;
			printf("Inserting after making %d moves\n",move);
			currNode = moveCursor(currNode,move);
			printf("Inserting %c after this character: %c\n",unNode.uNode->data,currNode->data);
		    currNode = insertCharAfter(currNode, unNode.uNode->data);
		    undoPop();
		    printf("%c\n",currNode->data);
			return currNode;
		default: 
		    return currNode;//throw error message

	}
}
