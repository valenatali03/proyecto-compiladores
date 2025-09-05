%{
	#include <stdio.h>
    #include <string.h>
	int yylex(void);
	extern void yyerror();
    extern FILE *yyin;
    extern int yylineno;
    extern int yycolumn;
%}

%token EXTERN BOOL PROGRAM ELSE THEN FALSE IF INTEGER RETURN TRUE VOID WHILE
%token BO BC PO PC SCOLON COMMA 
%token ID INT_LIT
%token ADD MULT DIV MINUS MOD COMP AND OR LT GT EQ NOT

%left OR
%left AND
%left GT LT COMP
%left ADD MINUS
%left MULT DIV MOD
%right NOT
%right UMINUS

%%

    var_decl: type ID EQ expr SCOLON
            ;

    type: INTEGER
        | BOOL
        ;

    params_call: /* vacío */
                | param_list
                ;

    param_list: expr
                | param_list COMMA expr
                ;


    method_call: ID PO params_call PC

    expr: ID
        | method_call
        | literal
        | expr OR expr
        | expr AND expr
        | expr GT expr
        | expr LT expr
        | expr COMP expr
        | expr ADD expr
        | expr MINUS expr
        | expr MULT expr
        | expr DIV expr
        | expr MOD expr
        | MINUS expr %prec UMINUS
        | NOT expr
        | PO expr PC
        ;

    literal: INT_LIT 
            | bool_lit
            ;

    bool_lit: TRUE 
            | FALSE
            ;
    
%%

int main(){
    yyparse();
}