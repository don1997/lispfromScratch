#include <stdio.h>
#include <stdlib.h>

//if compile on windows use these funcs.
#ifdef _WIN32
#include<string.h>

static char buffer[2048];

//fake readline function

char* readline(char* prompt){
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy,buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

//fake add_history functio
void add_history(char* unused){}

//otherwis include the editline headers

#else 

#include <editline/readline.h>

#endif


int main(int arc, char** argv)
{
	/* print version and exit infor */
	puts("Lisp Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	/*never ending loop*/
	while(1){

		//outputs our prompt and gathers user input
		char* input = readline("lispy> ");

		/* Add input ot history*/
		add_history(input);

		//echo input back to user
		printf("No your a %s", input);
		
		//free retrieved input
		free(input);
	
		printf("\n");
	}

	return 0;
}
