#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_STACK_SIZE 10

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

// structure for the undoRedoNode
struct undoRedoNode{
	char data;
	char operation;
	int moves;
};

struct undoRedoNode undo_stack[MAX_STACK_SIZE];
struct undoRedoNode redo_stack[MAX_STACK_SIZE];
int undoCursor = -1;
int redoCursor = -1;

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
struct node* goRight(struct node* currNode, int n)
{
	struct node* temp = currNode;
	while(temp->next != NULL && n>0){
		temp = temp->next;
		n--;
	}
	return temp;
}

//traverse the Linked List to the left, towards the head
struct node* goLeft(struct node* currNode, int n)
{
	struct node* temp = currNode;
	if(temp->prev != NULL && n<0){
		temp = temp->prev;
		n++;
	}
	return temp;
}

//API METHODS

int undoPush(struct node* currnode, char operation1, int newmoves)
{
	undoCursor++;
	if(undoCursor>MAX_STACK_SIZE-1) return -1;
	undo_stack[undoCursor].data=currnode->data;
	undo_stack[undoCursor].operation=operation1;
	undo_stack[undoCursor].moves=newmoves;
	return 1;
}

int redoPush(struct node* currnode, char operation1, int newmoves)
{
	redoCursor++;
	if(redoCursor>MAX_STACK_SIZE-1) return -1;
	redo_stack[redoCursor].data=currnode->data;
	redo_stack[redoCursor].operation=operation1;
	redo_stack[redoCursor].moves=newmoves;
	return 1;
}


int undoPop(struct node* currnode)
{
	if(undoCursor<0) return -1;
	evalURnode(&undo_stack[undoCursor], currnode);
	redoPush(currnode, undo_stack[undoCursor].operation, undo_stack[undoCursor].moves);
	undo_stack[undoCursor].data = '\0';
	undo_stack[undoCursor].operation = '\0';
	undo_stack[undoCursor].moves=0;
	undoCursor--;
	return 1;
}


int redoPop(struct node* currnode)
{
	if(redoCursor<0) return -1;
	evalURnode(&redo_stack[redoCursor],currnode);
	undoPush(currnode,redo_stack[redoCursor].operation,redo_stack[redoCursor].moves);
	redo_stack[redoCursor].data = '\0';
	redo_stack[redoCursor].operation = '\0';
	redo_stack[redoCursor].moves=0;
	redoCursor--;
	return 1;
}

//API for inserting a character after a given node
int insertCharAfter(struct node* currNode, char newData)
{
	if(currNode == NULL) return -1; //error has occurred
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	newNode->next = currNode->next;
	currNode->next = newNode;
	newNode->prev = currNode;
	if(newNode->next != NULL) newNode->next->prev = newNode;
	undoPush(newNode,'I',0); // when a new character is inserted it is pushed onto the undo stack
	return 1;
}

//API for deleting a character at the current node
int deleteChar(struct node* currNode)
{
	if(currNode == NULL) return -1; //error has occurred
	if(currNode->prev!=NULL) 
		currNode->prev->next = currNode->next;
	else
		currNode->next->prev = NULL;
	if(currNode->next!=NULL) 
		currNode->next->prev = currNode->prev;
	else
		currNode->prev->next = NULL;
	undoPush(currNode,'D',0); // when a new character is deleted it is pushed onto the undo stack
	free(currNode);
	return 1;
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
	while(head_ref->next != NULL){
		fputc(head_ref->data, file);
		head_ref = head_ref->next;
	}
	fclose(file);
}
//API for cursor handling combining goRight() and goLeft()
struct node* moveCursor(struct node* currNode, int n)
{
	struct node* temp = currNode;
	if(n>0){
		temp = goRight(temp, n);
		undoPush(temp,'M',n); // when a move is made, it is pushed onto the undo stack
	}
	else{
		temp = goLeft(temp, n);
		undoPush(temp,'M',n); // when a move is made, it is pushed onto the undo stack
	}
	return temp;
}
/*
	This function evaluates the undoRedoNode that has been popped off the top of the UR stack
*/
int evalURnode(struct undoRedoNode* undoRedoNode, struct node* currnode)
{
	// sees the operation in the undo node and takes action
	/*
		1. If the operation in the UR stack is insert then we delete the node at the current location.
		2. If the operation in the UR stack is delete then we create a new node and insert the node
		after the current location.
		3. If the operation is move then we check the number of movements and its sign. Next we move
		the cursor accordingly in the opposite direction.
	*/
	char operation1 = undoRedoNode->operation;
	switch(operation1){
		case 'I':
			return deleteChar(currnode); //return 1 for successful deletion and -1 for some error
		case 'D':
		    return insertCharAfter(currnode, undoRedoNode->data); //return 1 for successful insertion and -1 for some error
		case 'M':
		    moveCursor(currnode, -1 * (undoRedoNode->moves)); //Move Cursor to a specified position
			return 1;
		default: 
		    return -1;//throw error message
			// Committer note:
			// Maybe use a `throw` here? And why is there a break if there's a return?
			// Does no one review PRs anymore?
			// break;
	}
}


//The MAIN method
int main(int argc, char *argv[])
{
	//Load the whole file into a LinkedList and returns the tail pointer
	struct node* currNode = loadFileToList(argv[1]);
	insertCharAfter(currNode,'H');currNode = moveCursor(currNode,1);//currNode=currNode->next;
	insertCharAfter(currNode,'I');currNode = moveCursor(currNode,1);//currNode=currNode->next;
	displayAll(currNode);
	//undoPop(currNode);
	//displayAll(currNode);
	//deleteChar(currNode);
	//printf('\n');
	//displayAll(currNode);
	/*while(1){
		Start the front end program somehow.
		perform operations on the Linked List by capturing keystrokes on the frontend and mapping them to the suitable APIs
		provided by the backend.
		API 1. insertCharAfter(currNode,data), undoPush()
		API 2. deleteChar(currNode),
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



