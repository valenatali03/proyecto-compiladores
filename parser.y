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
    program: PROGRAM BO decls BC 
            | PROGRAM BO BC
        ;
    
    decls: decl
        | decls decl
        ;

    decl: var_decl
        | method_decl
        ;

    var_decls: var_decl
        | var_decls var_decl 
        ;

    var_decl: type ID EQ expr SCOLON
            ;

    type: INTEGER
        | BOOL
        | VOID
        ;


    block: BO var_decls statements BC 
        | BO statements BC
        | BO var_decls BC
        | BO BC
        ;

    statements: statement
            | statements statement
            ;

    statement: ID EQ expr SCOLON
            | method_call SCOLON
            | IF PO expr PC THEN block
            | IF PO expr PC THEN block ELSE block
            | WHILE PO expr PC block
            | RETURN SCOLON
            | RETURN expr SCOLON
            | block
            | SCOLON 
            ;

    method_decl: type ID PO params_decls PC block
            | type ID PO PC block
            | type ID PO params_decls PC EXTERN SCOLON
            | type ID PO PC EXTERN SCOLON
           ;


    params_decls : type ID
                     | params_decls COMMA type ID  
                     ;


    param_list: expr
                | param_list COMMA expr
                ;


    method_call: ID PO param_list PC
                | ID PO PC
               ;

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