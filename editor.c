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
	printf("Press S and Enter to save and Quit, Press I and Enter to start entering text and ~ and Enter to stop entering text.\n>> ");
	
	while(1){
		ch=getchar();
		if(ch == 'S' || ch == 's'){
			break;
		}
		else if(ch == 'I' || ch == 'i'){
			while(1){
				if((ch = getchar()) != '~')
					fputc( ch, file );
				else break;
			}
		}
		else{
			system("clear");
	                printf("Press S and Enter to save and Quit, Press I and Enter to start entering text and ~ and Enter to stop entering text.\n>> ");
		}
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
