%{
#include <stdio.h>
#include <stdbool.h>
#include "parser.tab.h"
int yycolumn = 1;
%}

%option yylineno

ALPHA               [a-zA-Z]
DIGIT               [0-9]
ID                  {ALPHA}({ALPHA}|{DIGIT}|_)*
INTEGER_LITERAL     {DIGIT}+
WHITE_SPACE         [ \t\r\n]+

%%

"extern"            {printf("Palabra reservada %s\n", yytext); return EXTERN;}
"bool"              {printf("Palabra reservada %s\n", yytext); return BOOL;}
"program"           {printf("Palabra reservada %s\n", yytext); return PROGRAM;}
"else"              {printf("Palabra reservada %s\n", yytext); return ELSE;}
"then"              {printf("Palabra reservada %s\n", yytext); return THEN;}
"false"             {printf("Palabra reservada %s\n", yytext); return FALSE;}
"if"                {printf("Palabra reservada %s\n", yytext); return IF;}
"integer"           {printf("Palabra reservada %s\n", yytext); return INTEGER;}
"return"            {printf("Palabra reservada %s\n", yytext); return RETURN;}
"true"              {printf("Palabra reservada %s\n", yytext); return TRUE;}
"void"              {printf("Palabra reservada %s\n", yytext); return VOID;}
"while"             {printf("Palabra reservada %s\n", yytext); return WHILE;}
"{"                 {printf("Llave abierta %s\n", yytext); return BO;}
"}"                 {printf("Llave cerrada %s\n", yytext); return BC;}
"("                 {printf("Parentesis abierto %s\n", yytext); return PO;}
")"                 {printf("Parentesis cerrado %s\n", yytext); return PC;}
";"                 {printf("Semicolon %s\n", yytext); return SCOLON;}
","                 {printf("Coma %s\n", yytext); return COMMA;}
"+"                 {printf("Operador %s\n", yytext); return ADD;}
"*"                 {printf("Operador %s\n", yytext); return MULT;}
"/"                 {printf("Operador %s\n", yytext); return DIV;}
"-"                 {printf("Operador %s\n", yytext); return MINUS;}
"%"                 {printf("Operador %s\n", yytext); return MOD;}
"=="                {printf("Operador %s\n", yytext); return COMP;}
"&&"                {printf("Operador %s\n", yytext); return AND;}
"||"                {printf("Operador %s\n", yytext); return OR;}
"<"                 {printf("Operador %s\n", yytext); return LT;}
">"                 {printf("Operador %s\n", yytext); return GT;}
"="                 {printf("Operador %s\n", yytext); return EQ;}
"!"                 {printf("Operador %s\n", yytext); return NOT;}
{INTEGER_LITERAL}   {printf("Literal %s\n", yytext); return INT_LIT;}
{ID}                {printf("Identificador %s\n", yytext); return ID;}
{WHITE_SPACE}       {}
.                   {printf("Error: caracter no reconocido '%s'\n", yytext);}

%%

void yyerror(){
    printf("Linea %d Col %d\n└──Error de sintaxis. \n", yylineno, yycolumn);
}
