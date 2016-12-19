/* 
	After two years of cursing vim and getting too friendly with GUI based text editors, 4 boys turn to the once forsaken text editor for 
inspiration and guidance on their final year project.
	Will the CLI text editor Gods look down with benevolence on the return of the prodigal sons, or have they prepared some sort of trial
which these 4 will have to undergo in order to prove themselves worthy? Stay tuned to know more!!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 10

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

struct URNODE{
	struct node *link;
	char operation;
};

int UFRONT=-1,UREAR=-1,RFRONT=-1,RREAR=-1;
struct URNODE UNDO[10],REDO[10];

//undo push

void Upush(){
	
}

//undo pop

void Upop(){
	
}

//redo push

void Rpush(){
	
}

//redo pop

void Rpop(){
	
}

//Undo

void undo(){
	
}

//Redo

void redo(){
	
}

//this link always point to first Link

void display(struct node* head_ref)
{
	struct node* temp = head_ref;
	while (temp != NULL)
    {
        printf("%c", temp->data);
        temp = temp->next;
    }
}

//push a character at the beginning of the Linked List
void push(struct node** head_ref, char newData)
{
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	
	newNode->next = (*head_ref);
	newNode->prev = NULL;
		
	if((*head_ref) != NULL)
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

//traverse the Linked List up, towards the head
void goUp(struct node* currNode)
{
	int i=0;
	if(currNode->prev != NULL && i<48)
		currNode = currNode->prev;
	i++;
	return;
}

//traverse the Linked List down, towards the tail
void goDown(struct node* currNode)
{
	int i=0;
	if(currNode->next != NULL && i<48)
		currNode = currNode->next;
	i++;
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

void writeToFile(char *filename)
{
	FILE *file = fopen(filename, "a+");;
	char ch;
	
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return;
	}

	// For now, if the user does not input anything other than I at the beginning, we will go on asking him to do so or use tilde to escape.
	printf("Press I and Enter to start entering text and ~ and Enter to stop entering text.\n>> ");
	
	while(1){
		ch=getchar();
		if(ch == 'I' || ch == 'i'){
			while(1){
				if((ch = getchar()) != '~')
					fputc( ch, file );
				else break;
			}
		}
		else if(ch == '~'){
			break;
		}
		else{
			printf("Press I and Enter to start entering text and ~ and Enter to stop entering text.\n>> ");
		}
	}
	
	fclose(file);
}

//take the characters from the file and insert them into a linkedlist while printing them at the same time

struct node* loadFromFile(char *filename)
{
	struct node* head = NULL;
	FILE *file;
	char ch;
	file=fopen(filename,"r");
	
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return;
	}
	
	while((ch=fgetc(file))!=EOF){
		append(&head, ch);
	}
	
	display(head);
	return head;
	fclose(file);
}

int main(int argc, char *argv[])
{
	// Open the file for writing
	writeToFile(argv[1]);
	
	//Load the editable Linked List
	struct node* head = loadFromFile(argv[1]);
		
	display(head);
	return 0;
}
// Green hoti Cabbage, Modi is savage xD
// Jisse dhaniya samjha, woh pudina nikla; jisse apna samjha woh kameena nikla xD
