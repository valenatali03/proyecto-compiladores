%{
	#include <stdio.h>
    #include <string.h>
	int yylex(void);
	extern void yyerror();
    extern FILE *yyin;
    extern int yylineno;
    extern int yycolumn;
%}

%code requires {
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include "includes/ast.h"
}

%union{
    int num;
    bool b;
    char* id;
    Tipo tipo;
    Arbol* ast;
    Parametro_Call* params_call;
    Parametro_Decl* params_decl;
}

%token T_EXTERN T_PROGRAM T_ELSE T_THEN T_FALSE T_IF T_RETURN T_TRUE T_WHILE
%token T_BO T_BC T_PO T_PC T_SCOLON T_COMMA 
%token T_ADD T_MULT T_DIV T_MINUS T_MOD T_COMP T_AND T_OR T_LT T_GT T_EQ T_NOT
%token <id> T_ID
%token <num> T_INT_LIT
%token <tipo> T_INTEGER T_BOOL T_VOID

%left T_OR
%left T_AND
%left T_GT T_LT T_COMP
%left T_ADD T_MINUS
%left T_MULT T_DIV T_MOD
%right T_NOT
%right UMINUS

%type <ast> expr literal bool_lit method_call
%type <params_call> param_list 
%type <params_decl> params_decls
%type <tipo> type

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


    params_decls: type T_ID                         {Info_ID *id = malloc(sizeof(Info_ID));
                                                     id->nombre = strdup($2);
                                                     id->tipo = $1;
                                                     $$ = agregar_param($$, id, FUNCION_DECL);
                                                    }
                | params_decls T_COMMA type T_ID    {Info_ID *id = malloc(sizeof(Info_ID));
                                                     id->nombre = strdup($4);
                                                     id->tipo = $3;
                                                     $$ = agregar_param($1, id, FUNCION_CALL);
                                                    }
                ;


    param_list: expr                    {$$ = agregar_param($$, $1, FUNCION_CALL);}
              | param_list T_COMMA expr {$$ = agregar_param($1, $3, FUNCION_CALL);}
              ;


    method_call: T_ID T_PO param_list T_PC  {$$ = crear_arbol_funcion_call($1, $3, yylineno, yycolumn, NULL, NULL);}
               | T_ID T_PO T_PC             {$$ = crear_arbol_funcion_call($1, NULL, yylineno, yycolumn, NULL, NULL);}
               ;

    expr: T_ID              {$$ = crear_arbol_id($1, yylineno, yycolumn, NULL, NULL);}
        | method_call       {$$ = $1;}
        | literal           {$$ = $1;}
        | expr T_OR expr    {$$ = crear_arbol_operador("||", BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_AND expr   {$$ = crear_arbol_operador("&&", BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_GT expr    {$$ = crear_arbol_operador(">", ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_LT expr    {$$ = crear_arbol_operador("<", ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_COMP expr  {$$ = crear_arbol_operador("==", VACIO, yylineno, yycolumn, $1, $3);}
        | expr T_ADD expr   {$$ = crear_arbol_operador("+", ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_MINUS expr {$$ = crear_arbol_operador("-", ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_MULT expr  {$$ = crear_arbol_operador("*", ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_DIV expr   {$$ = crear_arbol_operador("/", ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_MOD expr   {$$ = crear_arbol_operador("%", ENTERO, yylineno, yycolumn, $1, $3);}
        | T_MINUS expr %prec UMINUS {$$ = crear_arbol_operador("-", ENTERO, yylineno, yycolumn, $2, NULL);}
        | T_NOT expr        {$$ = crear_arbol_operador("!", BOOL, yylineno, yycolumn, $2, NULL);}
        | T_PO expr T_PC    {$$ = $2;}
        ;

    literal: T_INT_LIT {int* valor = malloc(sizeof(int));
                        *valor = $1;
                        $$ = crear_arbol_literal(valor, ENTERO, yylineno, yycolumn, NULL, NULL);}
           | bool_lit  {$$ = $1;}
           ;

    bool_lit: T_TRUE   {bool* valor = malloc(sizeof(bool));
                        *valor = true;
                        $$ = crear_arbol_literal(valor, BOOL, yylineno, yycolumn, NULL, NULL);}
            | T_FALSE  {bool* valor = malloc(sizeof(bool));
                        *valor = false;
                        $$ = crear_arbol_literal(valor, BOOL, yylineno, yycolumn, NULL, NULL);}
            ;
%%

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            printf("Error: No se puede abrir el archivo\n");
            return 1;
        }
        yyin = file;
    }
    
    int result = yyparse();
    
    if (result != 0) {
        printf("Error en el parsing\n");
    }
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return 0;
}