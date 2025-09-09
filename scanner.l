%{
#include <stdio.h>
#include <stdbool.h>
#include "parser.tab.h"
int yycolumn = 1;
%}

%option yylineno

ALPHA                           [a-zA-Z]
DIGIT                           [0-9]
ID                              {ALPHA}({ALPHA}|{DIGIT}|_)*
INTEGER_LITERAL                 {DIGIT}+

%%

"//".*                          {}           
"/*"([^*]|(\*+[^*/]))*"\*/"     {}
[ ]+        					{ yycolumn += yyleng; }
\t+         					{ yycolumn += 4 * yyleng; }
\n          					{ yycolumn = 1; }
\r          						
"extern"                        {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_EXTERN;}
"bool"                          {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_BOOL;}
"program"                       {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_PROGRAM;}
"else"                          {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_ELSE;}
"then"                          {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_THEN;}
"false"                         {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_FALSE;}
"if"                            {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_IF;}
"integer"                       {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_INTEGER;}
"return"                        {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_RETURN;}
"true"                          {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_TRUE;}
"void"                          {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_VOID;}
"while"                         {yycolumn += yyleng; printf("Palabra reservada %s\n", yytext); return T_WHILE;}
"{"                             {yycolumn += yyleng; printf("Llave abierta %s\n", yytext); return T_BO;}
"}"                             {yycolumn += yyleng; printf("Llave cerrada %s\n", yytext); return T_BC;}
"("                             {yycolumn += yyleng; printf("Parentesis abierto %s\n", yytext); return T_PO;}
")"                             {yycolumn += yyleng; printf("Parentesis cerrado %s\n", yytext); return T_PC;}
";"                             {yycolumn += yyleng; printf("Semicolon %s\n", yytext); return T_SCOLON;}
","                             {yycolumn += yyleng; printf("Coma %s\n", yytext); return T_COMMA;}
"+"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_ADD;}
"*"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_MULT;}
"/"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_DIV;}
"-"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_MINUS;}
"%"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_MOD;}
"=="                            {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_COMP;}
"&&"                            {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_AND;}
"||"                            {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_OR;}
"<"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_LT;}
">"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_GT;}
"="                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_EQ;}
"!"                             {yycolumn += yyleng; printf("Operador %s\n", yytext); return T_NOT;}
{INTEGER_LITERAL}               {yycolumn += yyleng; printf("Literal %s\n", yytext); return T_INT_LIT;}
{ID}                            {yycolumn += yyleng; printf("Identificador %s\n", yytext); return T_ID;}
.                               {yycolumn += yyleng; printf("Error: caracter no reconocido '%s'\n", yytext);}

%%

void yyerror(){
    printf("Linea %d Col %d\n└──Error de sintaxis. \n", yylineno, yycolumn);
}
