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

struct undoredonode{
	struct node *link;
	char operation;
};

int undofront=-1,undorear=-1,redofront=-1,redorear=-1;
struct undoredonode undostack[10],redostack[10];
int noofcharacters[1000][1],cursorpos=0;

//undo push

void undoPush(struct node *currNode,char opr){
	if(undofront==-1){
		undofront=undorear=0;
	}
	else{
		undofront=(undofront+1)%MAX;
		if(undofront==undorear){
			undorear++;
			free(undostack[undofront].link);
		}
	}
	undostack[undofront].link=currNode;
	undostack[undofront].operation=opr;
}

//undo pop

struct undoredonode undoPop(){
	int pos=0;
	/*if(undofront==-1){
		return ;
	}*/
	if(undofront==undorear){
		pos=undofront;
		undofront=undorear=-1;
	}
	else{
		undofront--;
		if(undofront<0){
			undofront=9;
		}
	}
	return undostack[pos];
}

//redo push

void redoPush(struct node *currNode,char opr){
	if(redofront==-1){
		redofront=redorear=0;
	}
	else{
		redofront=(redofront+1)%MAX;
		if(redofront==redorear){
			redorear++;
			free(redostack[redofront].link);
		}
	}
	redostack[redofront].link=currNode;
	redostack[redofront].operation=opr;
}

//redo pop

struct undoredonode redoPop(){
	int pos=0;
	/*if(undofront==-1){
		return ;
	}*/
	if(redofront==redorear){
		pos=redofront;
		redofront=redorear=-1;
	}
	else{
		redofront--;
		if(redofront<0){
			redofront=9;
		}
	}
	return redostack[pos];	
}

//Undo

void undo(){
	struct undoredonode unode=undoPop();
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
	struct undoredonode rnode=redoPop();
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

//insert a character before a given node
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


//insert a character after a given node
void insertAfter(struct node** currNode, char newData)
{
	if(*currNode == NULL ){
		printf("Error!!!\n");
		return; //error has occurred
	}
	
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	if((*currNode)->next == NULL){
		newNode->next = NULL;
		(*currNode)->next = newNode;
		newNode->prev = *currNode;
		*currNode=(*currNode)->next;
	}
	else{
		newNode->next = (*currNode)->next;
		newNode->prev = *currNode;
		
		(*currNode)->next->prev=newNode;
		(*currNode)->next = newNode;
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

//traverse The LinkedList Left Or Right N positions
int moveCursorN(int N,struct node* currNode){
	if(N>0){
		while(N>=0){
			if(currNode->next != NULL)
				currNode=currNode->next;
			else
				return -1;
			N--;
		}
	}
	else if(N<0){
		while(N<=0){
			if(currNode->next != NULL)
				currNode=currNode->next;
			else
				return -1;
			N++;
		}
	}
	return 0;
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
void operateonLinkedList(struct node** head_ref){
	struct node *currNode=NULL;currNode=*head_ref;
	char ch;
	//system ("/bin/stty raw");
	printf("\nPress I to start entering text and ~ to Exit A to move left D to move right.Press ENTER after any COMMAND\n>> ");
	while(1){
		ch=getchar();
		if(ch == 'I' || ch == 'i'){
			//Flushing STDIN buffer to delete the extra Undesired NewLine In the File And LinkedList
			fflush(stdin);
			while(1){
				if((ch = getchar()) != '~'){
					if(*head_ref != NULL){
						if(currNode->next == NULL)
							insertAfter( &currNode,ch );	
						else
							insertBefore( currNode,ch );
					}
					else{
						append(&currNode,ch);
						if(*head_ref==NULL)
							*head_ref=currNode;
						//currNode=NULL;
					}
					//printf("\n\nCurrentNode Data : %c\n\n",currNode->data);
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
				printf("Prev Character :%c\nNext Character :%c\n",currNode->prev->data,currNode->data);
			}
			//Flushing STDIN buffer to delete the extra Undesired NewLine
			//fflush(stdin);
		}
		else if(ch=='d'||ch=='D'){
			
			if(currNode->next!=NULL){
				currNode=currNode->next;
				printf("Prev Character :%c\nNext Character :%c\n",currNode->prev->data,currNode->data);
			}
			//Flushing STDIN buffer to delete the extra Undesired NewLine
			//fflush(stdin);
		}
		else{
			printf("\nPress I to start entering text and ~ to Exit A to move left D to move right.Press ENTER after any COMMAND\n>> ");
			//Flushing STDIN buffer to delete the extra Undesired NewLine
			//fflush(stdin);
		}
	}
	//system ("/bin/stty cooked");
}

//take the characters from the file and insert them into a linkedlist while printing them at the same time

struct node* loadFromFile(char *filename)
{
	struct node* head = NULL;
	FILE *file;
	char ch;
	file=fopen(filename,"r");
	
	if (!file){
		printf("File Not Found! Creating A New File! \n");
		file=fopen(filename,"a");
		if(!file){
			printf("File Creation Failed!Try executing the Program Again!\nExiting the Program with Code 0\n\a\a");
			exit(0);
		}
		fclose(file);
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
	operateonLinkedList(&head);
	
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
