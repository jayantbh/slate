#include "listfunc.h"

//The MAIN method
int main(int argc, char *argv[])
{
	struct node* currNode = loadFileToList(argv[1]);
	displayAll(currNode);
	printf("\n");
	/*We insert ABCDEF then we insert X after C and then we delete E and then we insert Y after F.
	moveCursor, insert and delete working*/
	currNode = insertCharAfter(currNode,'A');
    displayAll(currNode);
	printf("\n");
	currNode = insertCharAfter(currNode,'B');
    displayAll(currNode);
	printf("\n");
	currNode = insertCharAfter(currNode,'C');
    displayAll(currNode);
	printf("\n");
	currNode = insertCharAfter(currNode,'D');
    displayAll(currNode);
	printf("\n");
	currNode = insertCharAfter(currNode,'E');
    displayAll(currNode);
	printf("\n");
	currNode = insertCharAfter(currNode,'F');
    displayAll(currNode);
	printf("\n");
	currNode = moveCursor(currNode,-3);
	currNode = insertCharAfter(currNode,'X');
	displayAll(currNode);
	printf("\n");
	currNode = moveCursor(currNode,2);
	currNode = deleteChar(currNode);
	displayAll(currNode);
	printf("\n");
	currNode = moveCursor(currNode,1);
    currNode = insertCharAfter(currNode,'Y');
	displayAll(currNode);
	printf("\n");
	/*
    currNode = undo(currNode);
	move = 0;
    displayAll(currNode);
	printf("\n");
	currNode = insertCharAfter(currNode,'$');
	displayAll(currNode);
	printf("\n");
	currNode = deleteChar(currNode);
	displayAll(currNode);
	printf("\n");
	currNode = undo(currNode);
	move = 0;
	displayAll(currNode);
	printf("\n");*/
	writeBackToFile(currNode, argv[1]);
	return 0;
}



