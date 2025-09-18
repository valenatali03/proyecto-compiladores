%{
    #include <stdio.h>
    #include <string.h>
    #include "includes/tsim.h"
    int yylex(void);
    extern void yyerror();
    extern FILE *yyin;
    extern int yylineno;
    extern int yycolumn;
    Nivel *tabla = NULL;
%}

%code requires {
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include "includes/ast.h"
    #include "includes/semantico.h"
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

%type <ast> expr literal bool_lit method_call method_decl method_decls statement statements block var_decl var_decls program
%type <params_call> param_list 
%type <params_decl> params_decls
%type <tipo> type

%%
    program: T_PROGRAM T_BO var_decls method_decls T_BC {$$ = crear_arbol_nodo(PROGRAMA, yylineno, yycolumn, $3, $4); imprimir_vertical($$, "", 1);  tabla = crearTabla(); Nivel* nivelActual = tabla; analisis_semantico($$, nivelActual);}
           | T_PROGRAM T_BO var_decls T_BC              {$$ = crear_arbol_nodo(PROGRAMA, yylineno, yycolumn, $3, NULL); imprimir_vertical($$, "", 1); tabla = crearTabla(); Nivel* nivelActual = tabla; analisis_semantico($$, nivelActual);}
           | T_PROGRAM T_BO method_decls T_BC           {$$ = crear_arbol_nodo(PROGRAMA, yylineno, yycolumn, NULL, $3); imprimir_vertical($$, "", 1);  tabla = crearTabla(); Nivel* nivelActual = tabla; analisis_semantico($$, nivelActual);}
           | T_PROGRAM T_BO T_BC                        {$$ = NULL;}
           ;
    
    var_decls: var_decl     {$$ = crear_arbol_nodo(DECLARACIONES_VARIABLES, yylineno, yycolumn, $1, NULL);}
             | var_decls var_decl {$$ = crear_arbol_nodo(DECLARACIONES_VARIABLES, yylineno, yycolumn, $1, $2);}
             ;

    var_decl: type T_ID T_EQ expr T_SCOLON  {
                                                Arbol* id_arbol = crear_arbol_id($2, yylineno, yycolumn, NULL, NULL);
                                                id_arbol->info->id.tipo = $1;
                                                $$ = crear_arbol_nodo(DECLARACION_VARIABLE, yylineno, yycolumn, id_arbol, $4);
                                            }
            ;

    type: T_INTEGER
        | T_BOOL
        | T_VOID
        ;


    block: T_BO var_decls statements T_BC {$$ = crear_arbol_nodo(BLOQUE, yylineno, yycolumn, $2, $3);}
         | T_BO statements T_BC {$$ = crear_arbol_nodo(BLOQUE, yylineno, yycolumn, NULL, $2);}
         | T_BO var_decls T_BC  {$$ = NULL;}
         | T_BO T_BC            {$$ = NULL;}
         ;

    statements: statement   {$$ = crear_arbol_nodo(SENTENCIAS, yylineno, yycolumn, $1, NULL);}
              | statements statement {$$ = crear_arbol_nodo(SENTENCIAS, yylineno, yycolumn, $1, $2);}
              ;

    statement: T_ID T_EQ expr T_SCOLON  {
                                            Arbol* id_arbol = crear_arbol_id($1, yylineno, yycolumn, NULL, NULL);
                                            $$ = crear_arbol_nodo(ASIGNACION, yylineno, yycolumn, id_arbol, $3);
                                        }
             | method_call T_SCOLON     {$$ = $1;}
             | T_IF T_PO expr T_PC T_THEN block     {$$ = crear_arbol_if(IF, yylineno, yycolumn, $6, $3, NULL);}
             | T_IF T_PO expr T_PC T_THEN block T_ELSE block    {$$ = crear_arbol_if(IF, yylineno, yycolumn, $6, $3, $8);}
             | T_WHILE T_PO expr T_PC block     {$$ = crear_arbol_nodo(WHILE, yylineno, yycolumn, $3, $5);}
             | T_RETURN T_SCOLON                {$$ = crear_arbol_nodo(RETURN, yylineno, yycolumn, NULL, NULL);}
             | T_RETURN expr T_SCOLON           {$$ = crear_arbol_nodo(RETURN, yylineno, yycolumn, $2, NULL);}
             | block                            {$$ = $1;}
             | T_SCOLON                         {$$ = NULL;}
             ;

    method_decls: method_decl               {$$ = crear_arbol_nodo(DECLARACIONES_FUNCIONES, yylineno, yycolumn, $1, NULL);}
                | method_decls method_decl  {$$ = crear_arbol_nodo(DECLARACIONES_FUNCIONES, yylineno, yycolumn, $1, $2);}
                ;

    method_decl: type T_ID T_PO params_decls T_PC block {$$ = crear_arbol_funcion_decl($2, $1, $4, yylineno, yycolumn, $6, NULL);}
               | type T_ID T_PO T_PC block              {$$ = crear_arbol_funcion_decl($2, $1, NULL, yylineno, yycolumn, $5, NULL);}
               | type T_ID T_PO params_decls T_PC T_EXTERN T_SCOLON {$$ = crear_arbol_funcion_decl($2, $1, $4, yylineno, yycolumn, NULL, NULL);}
               | type T_ID T_PO T_PC T_EXTERN T_SCOLON  {$$ = crear_arbol_funcion_decl($2, $1, NULL, yylineno, yycolumn, NULL, NULL);}
               ;


    params_decls: type T_ID                         {Info_Union *param = malloc(sizeof(Info_Union));
                                                     param->id.nombre = strdup($2);
                                                     param->id.tipo = $1;
                                                     $$ = agregar_param(NULL, param, FUNCION_DECL);
                                                    }
                | params_decls T_COMMA type T_ID    {Info_Union *param = malloc(sizeof(Info_Union));
                                                     param->id.nombre = strdup($4);
                                                     param->id.tipo = $3;
                                                     $$ = agregar_param($1, param, FUNCION_DECL);
                                                    }
                ;


    param_list: expr                    {$$ = agregar_param(NULL, $1, FUNCION_CALL);}
              | param_list T_COMMA expr {$$ = agregar_param($1, $3, FUNCION_CALL);}
              ;


    method_call: T_ID T_PO param_list T_PC  {$$ = crear_arbol_funcion_call($1, $3, yylineno, yycolumn, NULL, NULL);}
               | T_ID T_PO T_PC             {$$ = crear_arbol_funcion_call($1, NULL, yylineno, yycolumn, NULL, NULL);}
               ;

    expr: T_ID              {$$ = crear_arbol_id($1, yylineno, yycolumn, NULL, NULL);}
        | method_call       {$$ = $1;}
        | literal           {$$ = $1;}
        | expr T_OR expr    {$$ = crear_arbol_operador("||", OPERADOR_BINARIO, BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_AND expr   {$$ = crear_arbol_operador("&&", OPERADOR_BINARIO, BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_GT expr    {$$ = crear_arbol_operador(">", OPERADOR_BINARIO, BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_LT expr    {$$ = crear_arbol_operador("<", OPERADOR_BINARIO, BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_COMP expr  {$$ = crear_arbol_operador("==", OPERADOR_BINARIO, BOOL, yylineno, yycolumn, $1, $3);}
        | expr T_ADD expr   {$$ = crear_arbol_operador("+", OPERADOR_BINARIO, ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_MINUS expr {$$ = crear_arbol_operador("-", OPERADOR_BINARIO, ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_MULT expr  {$$ = crear_arbol_operador("*", OPERADOR_BINARIO, ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_DIV expr   {$$ = crear_arbol_operador("/", OPERADOR_BINARIO, ENTERO, yylineno, yycolumn, $1, $3);}
        | expr T_MOD expr   {$$ = crear_arbol_operador("%", OPERADOR_BINARIO, ENTERO, yylineno, yycolumn, $1, $3);}
        | T_MINUS expr %prec UMINUS {$$ = crear_arbol_operador("-", OPERADOR_UNARIO, ENTERO, yylineno, yycolumn, $2, NULL);}
        | T_NOT expr        {$$ = crear_arbol_operador("!", OPERADOR_UNARIO, BOOL, yylineno, yycolumn, $2, NULL);}
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