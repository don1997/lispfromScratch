#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

/* Declare a buffer for user input of size 2048 */
static char input[2048];

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
