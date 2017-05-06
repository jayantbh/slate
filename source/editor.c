#include "listfunc.h"

//The MAIN method
int main(int argc, char *argv[])
{
	//Load the whole file into a LinkedList and returns the tail pointer
	struct node* currNode = loadFileToList(argv[1]);
	
	currNode = insertCharAfter(currNode,'H');

	//currNode = moveCursor(currNode,1);//currNode=currNode->next;
	currNode = insertCharAfter(currNode,'I');//currNode = moveCursor(currNode,1);//currNode=currNode->next;
	displayAll(currNode);
	//printf("\n");
    currNode = undo(currNode);
    displayAll(currNode);
	//printf("yo\n");
	currNode = insertCharAfter(currNode,'$');
	displayAll(currNode);
	//printf("yo2\n%c",currNode->data);
	currNode = deleteChar(currNode);
	displayAll(currNode);
	//printf("yo3\n");
	currNode = undo(currNode);
	displayAll(currNode);
	//printf("yo4\n");
	writeBackToFile(currNode, argv[1]);
	return 0;
}

