// The basic, most stripped down version of a Text Editor

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *file;
	char ch;
  
	// Open the file for writing
	file = fopen(argv[1], "a+");
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return -1;
	}

	// Read the data, save it to the file character by character and close the file pointer
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

	// Print from the file character by character
	file=fopen(argv[1],"r");
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
