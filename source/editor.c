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

struct urnode{
	struct node *link;
	char operation;
};

int ufront=-1,urear=-1,rfront=-1,rrear=-1;
struct urnode undostack[10],redostack[10];
int noofcharacters[1000][1],cursorpos=0;

//undo push

void undoPush(struct node *currNode,char opr){
	if(ufront==-1){
		ufront=urear=0;
	}
	else{
		ufront=(ufront+1)%MAX;
		if(ufront==urear){
			urear++;
			free(undostack[ufront].link);
		}
	}
	undostack[ufront].link=currNode;
	undostack[ufront].operation=opr;
}

//undo pop

struct urnode undoPop(){
	int pos=0;
	/*if(ufront==-1){
		return ;
	}*/
	if(ufront==urear){
		pos=ufront;
		ufront=urear=-1;
	}
	else{
		ufront--;
		if(ufront<0){
			ufront=9;
		}
	}
	return undostack[pos];
}

//redo push

void redoPush(struct node *currNode,char opr){
	if(rfront==-1){
		rfront=rrear=0;
	}
	else{
		rfront=(rfront+1)%MAX;
		if(rfront==rrear){
			rrear++;
			free(redostack[rfront].link);
		}
	}
	redostack[rfront].link=currNode;
	redostack[rfront].operation=opr;
}

//redo pop

struct urnode redoPop(){
	int pos=0;
	/*if(ufront==-1){
		return ;
	}*/
	if(rfront==rrear){
		pos=rfront;
		rfront=rrear=-1;
	}
	else{
		rfront--;
		if(rfront<0){
			rfront=9;
		}
	}
	return redostack[pos];	
}

//Undo

void undo(){
	struct urnode unode=undoPop();
	if(unode.operation=='d'){
		unode.link->prev->next=unode.link;
		unode.link->next->prev=unode.link;
		redoPush(unode.link,'i');
	}
	else{
		unode.link->prev=unode.link->next;
		unode.link->next=unode.link->prev;
		redoPush(unode.link,'d');
	}
}

//Redo

void redo(){
	struct urnode rnode=redoPop();
	if(rnode.operation=='d'){
		rnode.link->prev->next=rnode.link;
		rnode.link->next->prev=rnode.link;
		undoPush(rnode.link,'i');
	}
	else{
		rnode.link->prev=rnode.link->next;
		rnode.link->next=rnode.link->prev;
		undoPush(rnode.link,'d');
	}
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
void insertBefore(struct node* currNode, char newData)
{
	if(currNode == NULL){
		printf("Error!!!\n");
		return; //error has occurred
	}
	
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	if(currNode->prev == NULL){
		newNode->next = currNode;
		currNode->prev = newNode;
		newNode->prev = NULL;
	}
	else{
		newNode->next = currNode;
		newNode->prev = currNode->prev;
		
		currNode->prev->next=newNode;
		currNode->prev = newNode;
		}
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
	while(currNode->prev != NULL && i<48 && currNode->data != '\n'){
		currNode = currNode->prev;
		i++;
	}
	return;
}

//traverse the Linked List down, towards the tail
void goDown(struct node* currNode)
{
	int i=0;
	while(currNode->next != NULL && i<48 && currNode->data !='\n'){
		currNode = currNode->next;
		i++;
	}
	return;
}

//traverse the Linked List to the right, towards the tail
struct node* goRight(struct node* currNode)
{
	if(currNode->next != NULL)
		currNode = currNode->next;
	return currNode;
}

//traverse the Linked List to the left, towards the head
struct node* goLeft(struct node* currNode)
{
	if(currNode->prev != NULL)
		currNode = currNode->prev;
	return currNode;
}

void writeToFile(char *filename,struct node* head_ref)
{
	FILE *file = fopen(filename, "w+");;
	char ch;
	
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return;
	}
	
	struct node* temp = head_ref;
	while (temp != NULL)
    {
        fputc( temp->data, file );
		temp=temp->next;
    }
	
	fclose(file);
}

//modify the file using linkedlist
void operateonLinkedList(struct node* head_ref){
	struct node *currNode=NULL;currNode=head_ref;
	char ch;
	while(1){
		printf("Press I start entering text and ~ to Exit A to move left D to move right.\n>> ");
		ch=getchar();
		if(ch == 'I' || ch == 'i'){
			while(1){
				if((ch = getchar()) != '~'){
						insertBefore( currNode,ch );
				}
				else break;
			}
		}
		else if(ch == '~'){
			break;
		}
		else if(ch=='a'||ch=='A'){
			if(currNode->prev!=NULL){
				currNode=currNode->prev;
				//printf("Prev Character :%c\nNext Character :%c\n",currNode->prev->data,currNode->next->data);
			}
		}
		else if(ch=='d'||ch=='D'){
			if(currNode->next!=NULL){
				currNode=currNode->next;
				//printf("Prev Character :%c\nNext Character :%c\n",currNode->prev->data,currNode->next->data);
			}
		}
		else{
			//printf("Press I and Enter to start entering text and ~ and Enter to Exit.\n>> ");
		}
	}
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
		return head;
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
	
	//Load the editable Linked List
	struct node* head = loadFromFile(argv[1]);
	
	//operate on the linked list to insert or delete characters
	operateonLinkedList(head);
	
	//go to the new head location if any characters were inserted before current head position
	while(head->prev != NULL)
		head=head->prev;
	
	//Write linkedlist content to file
	writeToFile(argv[1],head);
	
	display(head);
	return 0;
}
// Green hoti Cabbage, Modi is savage xD
// Jisse dhaniya samjha, woh pudina nikla; jisse apna samjha woh kameena nikla xD
