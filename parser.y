%{
	#include <stdio.h>
    #include <string.h>
	int yylex(void);
	extern void yyerror();
    extern FILE *yyin;
    extern int yylineno;
    extern int yycolumn;
    extern FILE *out_sint;
%}

%token T_EXTERN T_BOOL T_PROGRAM T_ELSE T_THEN T_FALSE T_IF T_INTEGER T_RETURN T_TRUE T_VOID T_WHILE
%token T_BO T_BC T_PO T_PC T_SCOLON T_COMMA 
%token T_ID T_INT_LIT
%token T_ADD T_MULT T_DIV T_MINUS T_MOD T_COMP T_AND T_OR T_LT T_GT T_EQ T_NOT

%left T_OR
%left T_AND
%left T_GT T_LT T_COMP
%left T_ADD T_MINUS
%left T_MULT T_DIV T_MOD
%right T_NOT
%right UMINUS

%%
    program: T_PROGRAM T_BO var_decls method_decls T_BC 
           | T_PROGRAM T_BO var_decls T_BC 
           | T_PROGRAM T_BO method_decls T_BC 
           | T_PROGRAM T_BO T_BC
           ;
    
    var_decls: var_decl
             | var_decls var_decl 
             ;

    var_decl: type T_ID T_EQ expr T_SCOLON
            ;

    type: T_INTEGER
        | T_BOOL
        | T_VOID
        ;


    block: T_BO var_decls statements T_BC 
         | T_BO statements T_BC
         | T_BO var_decls T_BC
         | T_BO T_BC
         ;

    statements: statement
              | statements statement
              ;

    statement: T_ID T_EQ expr T_SCOLON
             | method_call T_SCOLON
             | T_IF T_PO expr T_PC T_THEN block
             | T_IF T_PO expr T_PC T_THEN block T_ELSE block
             | T_WHILE T_PO expr T_PC block
             | T_RETURN T_SCOLON
             | T_RETURN expr T_SCOLON
             | block
             | T_SCOLON 
             ;

    method_decls: method_decl
                | method_decls method_decl
                ;

    method_decl: type T_ID T_PO params_decls T_PC block
               | type T_ID T_PO T_PC block
               | type T_ID T_PO params_decls T_PC T_EXTERN T_SCOLON
               | type T_ID T_PO T_PC T_EXTERN T_SCOLON
               ;


    params_decls: type T_ID
                | params_decls T_COMMA type T_ID  
                ;


    param_list: expr
              | param_list T_COMMA expr
              ;


    method_call: T_ID T_PO param_list T_PC
               | T_ID T_PO T_PC
               ;

    expr: T_ID
        | method_call
        | literal
        | expr T_OR expr
        | expr T_AND expr
        | expr T_GT expr
        | expr T_LT expr
        | expr T_COMP expr
        | expr T_ADD expr
        | expr T_MINUS expr
        | expr T_MULT expr
        | expr T_DIV expr
        | expr T_MOD expr
        | T_MINUS expr %prec UMINUS
        | T_NOT expr
        | T_PO expr T_PC
        ;

    literal: T_INT_LIT 
           | bool_lit
           ;

    bool_lit: T_TRUE 
            | T_FALSE
            ;
%%

void yyerror(const char *s) {
    fprintf(out_sint,"Linea %d Col %d\n└──Error de sintaxis.\n", yylineno, yycolumn);
}