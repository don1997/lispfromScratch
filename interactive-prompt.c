#include <stdio.h>

/* Declare a buffer for user input of size 2048 */
static char input[2048];

int main(int arc, char** argv)
{
	/* print version and exit infor */
	puts("Lisp Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	/*never ending loop*/
	while(1){

		//outputs our prompt
		fputs("lispy> ", stdout);
		
		//Read line of user input max size 2048
		fgets(input, 2048, stdin);

		//echo input back to user
		printf("No your a %s", input);
		
	}

	return 0;
}
