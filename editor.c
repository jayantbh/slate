/* 
	After two years of cursing vim and getting too friendly with GUI based text editors, 4 boys turn to the once forsaken text editor for 
inspiration and guidance on their final year project.
	Will the CLI text editor Gods look down with benevolence on the return of the prodigal sons, or have they prepared some sort of trial
which these 4 will have to undergo in order to prove themselves worthy? Stay tuned to know more!!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void writeToFile(char *filename)
{
	FILE *file = fopen(filename, "a+");;
	char ch;
	
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return;
	}

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
}

void printFromFile(char *filename)
{
	FILE *file;
	char ch;
	file=fopen(filename,"r");
	
	if (!file){
		printf("File could not be opened. Press any key to continue! \n\a\a");
		getchar();
		return;
	}
	
	while((ch=fgetc(file))!=EOF){
		printf("%c",ch);
	}
	
	fclose(file);
}

int main(int argc, char *argv[])
{
	// Open the file for writing
	writeToFile(argv[1]);

	// Print from the file
	printFromFile(argv[1]);
	
	return 0;
}
// Green hoti Cabbage, Modi is savage xD
