#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STACK_SIZE 100
#define KB 1024

struct node{
	char data;
	struct node *next;
	struct node *prev;
};

struct undoNode{
	struct node* uNode;
	char op;
};

struct undoNode undo_stack[MAX_STACK_SIZE];
int undoCursor = -1;

int positionArray[100];
int indexOfPos = 0;


int calculateMoves(struct node* currNode,struct node* nextNode);
struct node* getHead(struct node* currNode);
void displayAll(struct node* currNode);
void displayCurrN(struct node* currNode, int N);
struct node* goRight(struct node* currNode, int n);
struct node* goLeft(struct node* currNode, int n);
struct node* moveCursor(struct node* currNode, int n);
struct node* insertCharAfter(struct node* currNode, char newData);
struct node* insertCharBefore(struct node* currNode, char newData);
char * getFileContents(struct node* currNode);
struct node* deleteChar(struct node* currNode);
struct node* loadFileToList(char *filename);
struct node* getEmptyList(void);
void writeBackToFile(struct node* head_ref, char *filename);

int undoPush(struct node* currNode, char operation);
struct undoNode undoPop(void);
int undo(struct node** currNode);

void initializePositionArray(void);
void KMPSearch(char *pat, char *txt);
void computeLPSArray(char *pat, int M, int *lps);
char* convertToString(struct node* currNode);
void find(char* pattern,struct node* currNode);
void findAndReplace(char* pattern,char* replace,struct node* currNode);
void findAndReplaceAll(char* pattern,char* replace,struct node* currNode);

int calculateMoves(struct node* currNode,struct node* nextNode){
	int movesLeft = 0,movesRight = 0;
	struct node *tempLeft = currNode,*tempRight = currNode;
	while(1){
		if(tempLeft!=NULL){
			tempLeft = tempLeft->prev;
			movesLeft--;
			if(tempLeft == nextNode)
				return movesLeft;
		}
		if(tempRight!=NULL){
			tempRight = tempRight->next;
			movesRight++;
			if(tempRight == nextNode)
				return movesRight;
		}
	}
	
}

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
	}
	else{
		temp = goLeft(temp, n);
	}
	return temp;
}
//API for inserting a character after a given node
struct node* insertCharAfter(struct node* currNode, char newData)
{
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	newNode->next = currNode->next;
	currNode->next = newNode;
	newNode->prev = currNode;
	if(newNode->next != NULL) newNode->next->prev = newNode;
	currNode=newNode;
	
	undoPush(newNode,'I'); // when a new character is inserted it is pushed onto the undo stack
	return currNode;
}

//API for inserting a character before a given node
struct node* insertCharBefore(struct node* currNode, char newData)
{
	// If Linked list is empty then create a new node and return this as current node
	if(currNode->data == '\0'){
		struct node* newNode = (struct node*) malloc(sizeof(struct node));
		newNode->data = newData;
		newNode->prev= NULL;
		newNode->next= NULL;
		return newNode;
	}

	//anywhere else
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->data = newData;
	if(currNode->prev!=NULL){
		newNode->prev = currNode->prev;
		currNode->prev->next = newNode;
	}
	else 
		newNode->prev = NULL;
	newNode->next = currNode;
	currNode->prev = newNode;

	currNode=newNode;
	
	undoPush(newNode,'I'); // when a new character is inserted it is pushed onto the undo stack
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
	undoPush(currNode,'D'); // when a new character is deleted it is pushed onto the undo stack
	//free(currNode);
	return prevNode;
}

/*
	Take the characters from the file and insert them into a string (fast loading).
	Now construct the linkedlist from the string.
	Return the head pointer of the Linked List.
*/
struct node* loadFileToList(char *filename)
{
	if(filename == NULL){
		printf("Enter a valid filename!\n");
		exit(0);
	}
	struct node* head = (struct node*) malloc(sizeof(struct node)); struct node *memory1,*currNode;
	head->prev = head->next = NULL;
	head->data = '\r';
	currNode = head;
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
	while(list[i]!='\0'){
		currNode = insertCharAfter(currNode,list[i]);
	   	i++;
	}
	printf("\nFile Loaded!\n");
	return getHead(head);
}

//returns a list with 2 nodes
struct node* getEmptyList()
{
    struct node* head = (struct node*) malloc(sizeof(struct node));
    struct node* body = (struct node*) malloc(sizeof(struct node));
    head->data = '\r';
    head->prev = NULL;
    head->next = body;
    body->prev = head;
    body->next = NULL;
    return getHead(body);
}

//write the linked list back into the file
void writeBackToFile(struct node* head_ref, char *filename)
{
	head_ref = getHead(head_ref);
	head_ref = head_ref->next;
	FILE *file = fopen(filename,"w");
	if (!file){		printf("File could not be opened. Press any key to continue! \n\a\a");		getchar();		return;	}
	while(head_ref != NULL){
		fputc(head_ref->data, file);
		head_ref = head_ref->next;
	}
	fclose(file);
}

char * getFileContents(struct node* currNode) {
    struct node* temp = getHead(currNode);
    char * list = (char *) calloc((size_t) 200 * KB, sizeof(char));
    int i = 0;
    while (temp != NULL)
    {
        list[i] = temp->data;
        temp = temp->next;
        i++;
    }
    return list;
}

int undoPush(struct node* currNode, char operation)
{
	undoCursor++;
	if(undoCursor>MAX_STACK_SIZE-1) return -1;
	undo_stack[undoCursor].uNode=currNode;
	undo_stack[undoCursor].op=operation;
	return 1;
}

struct undoNode undoPop(void)
{
	//if(undoCursor<0) return NULL;
	struct undoNode unNode = undo_stack[undoCursor];
	undoCursor--;
	return unNode;
}

int undo(struct node** currNode)
{
	struct undoNode unNode = undoPop();
	//if(unNode == NULL) return -1;
	char oper = unNode.op;
	struct node* nextNode;
	int move=0;
	switch(oper){
		case 'I':
			nextNode = deleteChar(unNode.uNode); //return 1 for successful deletion and -1 for some error
			move = calculateMoves(*currNode,nextNode);
			*currNode = nextNode;
			undoPop();
			return -1;
		case 'D':
//		    printf("Inserting %c after this character: %c\n",unNode.uNode->data,unNode.uNode->prev->data);
		    nextNode = insertCharAfter(unNode.uNode->prev, unNode.uNode->data); //return 1 for successful insertion and -1 for some error
			move = calculateMoves(*currNode,nextNode);
			*currNode = nextNode;
		    undoPop();
			return +1;
		default: 
		    return move;//throw error message

	}
}

void initializePositionArray()
{
    for(indexOfPos = 0;indexOfPos<100;indexOfPos++){
        positionArray[indexOfPos]=-1;
    }
    indexOfPos = 0;
}
// Finds occurrences of txt[] in pat[] and stores the indices in positionArray
void KMPSearch(char *pat, char *txt)
{
    int M = strlen(pat);
    int N = strlen(txt);
    int lps[M];
    computeLPSArray(pat, M, lps);
     int i = 0;  // index for txt[]
    int j  = 0;  // index for pat[]
    while (i < N)
    {
        if (pat[j] == txt[i])
        {
            j++;
            i++;
        }
        if (j == M)
        {
            positionArray[indexOfPos]=(i-j);
            indexOfPos++;
            j = lps[j-1];
        }
        // mismatch after j matches
        else if (i < N && pat[j] != txt[i])
        {
            if (j != 0)
                j = lps[j-1];
            else
                i = i+1;
        }
    }
}
 
// Fills lps[] for given patttern pat[0..M-1]
void computeLPSArray(char *pat, int M, int *lps)
{
    // length of the previous longest prefix suffix
    int len = 0;
    lps[0] = 0; // lps[0] is always 0
    // the loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M)
    {
        if (pat[i] == pat[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else // (pat[i] != pat[len])
        {
            if (len != 0)
                len = lps[len-1];
            else // if (len == 0)
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}

char* convertToString(struct node* currNode)
{
	char* text = malloc(100000);
	struct node* head = getHead(currNode);
	int n = 0;
	while(head->next!=NULL){
		text[n++] = head->data;
		head = head->next;
	}
	//text[n]='\0';
	return text;
}

void find(char* pattern,struct node* currNode)
{
	char* text = convertToString(currNode);
//	printf("\n%s\n",text);
	initializePositionArray();
	KMPSearch(pattern,text);
}
void findAndReplace(char* pattern,char* replace,struct node* currNode)
{
	find(pattern,currNode);
	int h=0;
	int lengthOfPos = 0;
	for(;positionArray[h]!=-1;h++) lengthOfPos++;
	int index = 0;
	struct node* temp = getHead(currNode);
	int i=0,pos=positionArray[0];
	int j = pos;
	int patternLength = strlen(pattern);
	int replaceLength = strlen(replace);
	
	while(j-->0){
		temp=temp->next;
	}
	struct node* begin = temp;
	//printf("\n%c\n",begin->data);
	int ptr1 = patternLength-1;
	while(ptr1-->0){
		begin = begin->next;
	}

	ptr1 = patternLength;
	while(ptr1-->0){
		begin = deleteChar(begin);
	}
    //printf("\n%c\n",begin->data);
	ptr1 = replaceLength;
	for(i = 0;i < ptr1;i++){
		begin = insertCharAfter(begin,replace[i]);
	}
	index++;
}
void findAndReplaceAll(char* pattern,char* replace,struct node* currNode)
{
	find(pattern,currNode);
	int h=0;
	int lengthOfPos = 0;
	for(;positionArray[h]!=-1;h++) lengthOfPos++;
	int index = 0;
	struct node* temp = getHead(currNode);
	int i=0,pos=positionArray[0];
	int j = pos;
	int patternLength = strlen(pattern);
	int replaceLength = strlen(replace);
	
	while(j-->0){
		temp=temp->next;
	}
	struct node* begin = temp;
	//printf("\n%c\n",begin->data);
	int ptr1 = patternLength-1;
	while(ptr1-->0){
		begin = begin->next;
	}

	ptr1 = patternLength;
	while(ptr1-->0){
		begin = deleteChar(begin);
	}
    //printf("\n%c\n",begin->data);
	ptr1 = replaceLength;
	for(i = 0;i < ptr1;i++){
		begin = insertCharAfter(begin,replace[i]);
	}
	index++;
	while(index<lengthOfPos){
	    find(pattern,currNode);
	    int h=0;
	    for(;positionArray[h]!=-1;h++); //printf("%d\n",positionArray[h]);
	    
		temp = getHead(currNode);
		i=0,pos=positionArray[0];
		j = pos;
		
		while(j-->0){
			temp=temp->next;
		}
		begin = temp;
		//printf("\n%c\n",begin->data);
		ptr1 = patternLength-1;
		while(ptr1-->0){
			begin = begin->next;
		}

		ptr1 = patternLength;
		while(ptr1-->0){
			begin = deleteChar(begin);
		}
    	//printf("\n%c\n",begin->data);
		ptr1 = replaceLength;
		for(i = 0;i < ptr1;i++){
			begin = insertCharAfter(begin,replace[i]);
		}
		index++;
	}

}
