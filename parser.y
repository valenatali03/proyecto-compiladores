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
    program: PROGRAM BO var_decls method_decls BC 
        ;

    var_decls: /* vacío */
        | var_decls var_decl 
        ;

    var_decl: type ID EQ expr SCOLON
            ;

    function_type: INTEGER
                | BOOL
                | VOID
                ;

    block: BO var_decls statements BC 
        ;

    statements: /* vacío */
            | statement statements
            ;

    statement: ID EQ expr SCOLON
            | method_call SCOLON
            | IF PO expr PC THEN block
            | IF PO expr PC THEN block ELSE block
            | WHILE PO expr PC block
            | RETURN SCOLON
            | RETURN expr SCOLON
            | block
            ;
            /*| SCOLON ? */

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