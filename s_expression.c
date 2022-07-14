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
enum{LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_ERR};

//Declare new lval struct
typedef struct lval{
	int type;
	long num;
	//errror and symbol types have some string data
	char* err;
	char* sym;

	//count and pointer to a list of "lval*"
	int count;
	struct lval** cell;
} lval;

//construct a pointer to a new number lval
lval* lval_num(long x){
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num = x;
	return v;
}

//construct a pointer to a new error lval
lval* lval_err(char* m){
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_ERR;
	v->err = malloc(strlen(m)+1);
	strcpy(v->err,m);
	return v;
}


//construct pointer to new symbol lval
lval* lval_sym(char* s){
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SYM;
	v->sym = malloc(strlen(s)+1);
	strcpy(v->sym, s);
	return v;
}

//pointer to new empty sexpr lval
lval* lval_sexpr(void){
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

//free lval* pointer
void lval_del(lval* v){

	switch (v->type){
		//Do nothing special for number type
		case LVAL_NUM: break;

		//for Err or Sym free the string data
		case LVAL_ERR: free(v->err); break;
		case LVAL_SYM: free(v->sym); break;

		//if sexpr then delete all elements inside
		case LVAL_SEXPR:
			for(int i = 0; i < v->count; i++){

				lval_del(v->cell[i]);
			}

		//also free mem alloc to conatin the pointers
		//
		free(v->cell);

		break;
	}

	//free mem allocated for lval struct itself
	free(v);

}



//read num
lval* lval_read_num(mpc_ast_t* t){

	errno = 0;
	long x = strtol(t->contents, NULL, 10);
	return errno != ERANGE ?
		lval_num(x) : lval_err("invalid number");

}

//lval_add
lval* lval_add(lval* v,lval* x){

        v->count++;
        v->cell = realloc(v->cell,sizeof(lval*) * v->count);
        v->cell[v->count-1] = x;
        return v;
}

//lval read
lval* lval_read(mpc_ast_t* t){

	//if symol or num return conversion to that type
	if(strstr(t->tag, "number")){return lval_read_num(t);}
	if(strstr(t->tag, "symbol")){return lval_sym(t->contents);}
	//if root (>) or sexpr then create empty list
	lval* x = NULL;
	if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  	if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }

  	/* Fill this list with any valid expression contained within */
  	for (int i = 0; i < t->children_num; i++) {
    		if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    		if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    		if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    		x = lval_add(x, lval_read(t->children[i]));
  	}

	
	return x;
}

void lval_print(lval* v); 
void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {

    /* Print Value contained within */
    lval_print(v->cell[i]);

    /* Don't print trailing space if last element */
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}




//print lval
void lval_print(lval* v){
	switch(v->type){
	
		case LVAL_NUM: printf("%li", v->num); break;
		case LVAL_ERR: printf("Error: %s", v->err); break;
		case LVAL_SYM: printf("%s", v->sym); break;
		case LVAL_SEXPR: lval_expr_print(v, '(', ')');break;


	}
}



/* Print an "lval" followed by a newline */
void lval_println(lval* v) { lval_print(v); putchar('\n'); }

int main(int arc, char** argv)
{
	
  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr    = mpc_new("sexpr");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  

      mpca_lang(MPCA_LANG_DEFAULT,
      "                                          \
      number : /-?[0-9]+/ ;                    \
      symbol : '+' | '-' | '*' | '/' ;         \
      sexpr  : '(' <expr>* ')' ;               \
      expr   : <number> | <symbol> | <sexpr> ; \
      lispy  : /^/ <expr>* /$/ ;               \
      ",
      Number, Symbol, Sexpr, Expr, Lispy);



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
	lval* x = lval_read(r.output);
	lval_println(x);
	lval_del(x);
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
        mpc_cleanup(5, Number,Symbol, Sexpr, Expr, Lispy);
	printf("done\n");
	return 0;
}
