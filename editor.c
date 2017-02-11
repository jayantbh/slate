#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

// structure for the URnode
struct URnode{
	char data;
	char operation;
	char number;
};

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
	while (temp->next != NULL)
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
void goRight(struct node* currNode, int n)
{
	if(currNode->next != NULL && n>0){
		currNode = currNode->next;
		n--;
	}
	return;
}

//traverse the Linked List to the left, towards the head
void goLeft(struct node* currNode, int n)
{
	if(currNode->prev != NULL && n<0){
		currNode = currNode->prev;
		n++;
	}
	return;
}

//API METHODS

//API for cursor handling combining goRight() and goLeft()
void moveCursor(struct node* currNode, int n)
{
	if(n>0) goRight(currNode, n);
	else goLeft(currNode, n);
	return;
}

//API for inserting a character after a given node
void insertCharAfter(struct node* currNode, char newData)
{
	if(currNode == NULL) return; //error has occurred
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	newNode->next = currNode->next;
	currNode->next = newNode;
	newNode->prev = currNode;
	if(newNode->next != NULL) newNode->next->prev = newNode;
	return;
}

//API for deleting a character at the current node
void deleteChar(struct node* currNode)
{
	if(currNode == NULL) return; //error has occurred
	if(currNode->prev!=NULL) currNode->prev->next = currNode->next;
	if(currNode->next!=NULL) currNode->next->prev = currNode->prev;
	currNode->prev = NULL;
	currNode->next = NULL;
	return;
}

/*
	Take the characters from the file and insert them into a string (fast loading).
	Now construct the linkedlist from the string.
	Return the head pointer of the Linked List.
*/
struct node* loadFileToList(char *filename)
{
	struct node* head = (struct node*) malloc(sizeof(struct node)); struct node* memory1;
	head->prev = NULL, head->next = NULL;
	char* list = malloc(100000); char ch;
	int n = 0, i=0;
	FILE *file;
	file = fopen(filename,"r+");
	if (!file){
		file = fopen(filename,"w+");
		if(!file){
			printf("File could not be opened. Press any key to continue! \n\a\a");
			getchar();
			return;
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
	while(head_ref->next != NULL){
		fputc(head_ref->data, file);
		head_ref = head_ref->next;
	}
	fclose(file);
}

/*
	This function evaluates the URnode that has been popped off the top of the UR stack
*/
void evalURnode(struct URnode* urnode, struct node* newnode)
{
	char data1 = urnode->data;
	char operation1 = urnode->operation;
	// sees the operation in the undo node and takes action
	/*
		1. If the operation in the UR stack is insert then we delete the node at the current location.
		2. If the operation in the UR stack is delete then we create a new node and insert the node
		after the current location.
		3. If the operation is move then we check the number of movements and its sign. Next we move
		the cursor accordingly in the opposite direction.
	*/
	switch(operation1){
		case 'I':
			
			break;
		case 'D':
			break;
		case 'M':
			break;
		default: //throw error message
			break;
	}
}

//The MAIN method
int main(int argc, char *argv[])
{
	//Load the whole file into a LinkedList and returns the tail pointer
	struct node* currNode = loadFileToList(argv[1]);
	/*while(1){
		Start the front end program somehow.
		perform operations on the Linked List by capturing keystrokes on the frontend and mapping them to the suitable APIs
		provided by the backend.
		API 1. insertCharAfter(currNode,data)
		API 2. deleteChar(currNode)
		API 3. moveCursor(currNode, N)
		API 4. displayAll(currNode) --> display from start to end
		API 4. displayCurrN(currNode,N) --> displays the Linked List from start to end
		API 5. undo()
		API 6. redo()
	}*/
	//Write List back into the file
	writeBackToFile(currNode, argv[1]);
	return 0;
}
