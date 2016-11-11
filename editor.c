// The basic, most stripped down version of a Text Editor

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	FILE *file;
	char ch;
  
	// Open the file for writing
	file = fopen("records.txt", "w");
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return -1;
	}

	// Read the data, save it to the file character by character and close the file pointer
	printf("Enter text after the prompt. Enter ~ and press enter to stop entering text.\n>> ");
	
	while(1){
		if((ch = getchar()) != '~')
			fputc( ch, file );
		else break;
	}
	fclose(file);

	// Print from the file character by character
	
	file=fopen("records.txt","rt");
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return -1;
	}
	while((ch=fgetc(file))!=EOF){
		printf("%c",ch);
	}
	fclose(file);
	return 0;
}