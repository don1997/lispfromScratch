#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

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

//Create enum of possible lval types
enum{LVAL_NUM, LVAL_ERR};

//Create enum of possible error types
enum{LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM};

//Declare new lval struct
typedef struct{
	int type;
	long num;
	int err;
} lval;

//Create a new number type lval 
lval lval_num(long x){
	lval v;
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

//create new error type lval
lval lval_err(int x){
	lval v;
	v.type = LVAL_ERR;
	v.err = x;
	return v;
}

//print lval
void lval_print(lval v){
	switch(v.type){
		//in case type is a number print it
		//then break out of switch
		case LVAL_NUM: printf("%li", v.num); break;

		//in case type is error
		case LVAL_ERR:
			//check what type error and print it
			if(v.err ==LERR_DIV_ZERO){
				printf("Error: Division by zero");
			}
			if(v.err ==LERR_BAD_OP){
				printf("Error: Invalid Operator");
			}
			if(v.err ==LERR_BAD_NUM){
				printf("Error: Invalid Number");
			}
			break;
	}
}

/* Print an "lval" followed by a newline */
void lval_println(lval v) { lval_print(v); putchar('\n'); }

//use operator string so see wich operation to perform
lval eval_op(lval x, char* op, lval y){
	
	//if either value is an error return it
	if(x.type == LVAL_ERR){ return x;}
	if(y.type == LVAL_ERR){ return y;}
	//otherwise do math on number vals
	if(strcmp(op, "+") == 0) {return lval_num(x.num + y.num);}
	if(strcmp(op, "-") == 0) {return lval_num(x.num - y.num);}
	if(strcmp(op, "*") == 0) {return lval_num(x.num * y.num);}
	if(strcmp(op, "/") == 0) {

		//if second operand is zero return error
		return y.num == 0
			? lval_err(LERR_DIV_ZERO)
			: lval_num(x.num / y.num);
	}

return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t){

	//if tagged as number return it directly
	if(strstr(t->tag, "number")){
		
		//check if ther is some error in conversion
		errno = 0;
		long x  = strtol(t-> contents, NULL, 10);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);

	}


  	/* The operator is always second child. */
  	char* op = t->children[1]->contents;

  	/* We store the third child in `x` */
	lval x = eval(t->children[2]);

  	/* Iterate the remaining children and combining. */
  	int i = 3;
 	while (strstr(t->children[i]->tag, "expr")) {
    		
		x = eval_op(x, op, eval(t->children[i]));
		i++;
  	}

  return x;
}


int main(int arc, char** argv)
{
	
  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);

	/* print version and exit infor */
	puts("Lisp Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	/*never ending loop*/
	while(1){

	//outputs our prompt and gathers user input
	char* input = readline("lispy> ");

	/* Add input ot history*/
	add_history(input);

        /* Attempt to parse the user input */
	mpc_result_t r;
    	if (mpc_parse("<stdin>", input, Lispy, &r)) {
      	/* On success print and delete the AST */
//      		mpc_ast_print(r.output);
//      		mpc_ast_delete(r.output);
		
		//print result instead of entire tree
		lval result = eval(r.output);
		lval_println(result);
		mpc_ast_delete(r.output);
		
    	} else {
      	/* Otherwise print and delete the Error */
      	mpc_err_print(r.error);
      	mpc_err_delete(r.error);
    	
	}
	
		//free retrieved input
		free(input);
	
		printf("\n");
	}
	
	 
        /* Undefine and delete our parsers */
        mpc_cleanup(4, Number, Operator, Expr, Lispy);
	printf("done\n");
	return 0;
}
