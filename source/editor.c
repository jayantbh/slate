#include "listfunc.h"
//The MAIN method
int main(int argc, char *argv[])
{
	//Load the whole file into a LinkedList and returns the tail pointer
	struct node* currNode = loadFileToList(argv[1]);
	
	currNode = insertCharAfter(currNode,'H');

	//currNode = moveCursor(currNode,1);//currNode=currNode->next;
	currNode = insertCharAfter(currNode,'I');//currNode = moveCursor(currNode,1);//currNode=currNode->next;

	currNode = insertCharAfter(currNode,'$');
	deleteChar(currNode);
	displayAll(currNode);
	
	writeBackToFile(currNode, argv[1]);
	return 0;
}
