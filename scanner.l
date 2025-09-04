%{
#include <stdio.h>
#include <stdbool.h>
//#include "parser.tab.h"
int yycolumn = 1;
%}

%option yylineno

ALPHA               [a-zA-Z]
DIGIT               [0-9]
ID                  {ALPHA}({ALPHA}|{DIGIT}|_)*
INTEGER_LITERAL     {DIGIT}+
BOOL_LITERAL        "true"|"false"
WHITE_SPACE         [ \t\r\n]+

%%

"extern"            {printf("Palabra reservada %s\n", yytext);}
"bool"              {printf("Palabra reservada %s\n", yytext);}
"program"           {printf("Palabra reservada %s\n", yytext);}
"else"              {printf("Palabra reservada %s\n", yytext);}
"then"              {printf("Palabra reservada %s\n", yytext);}
"false"             {printf("Palabra reservada %s\n", yytext);}
"if"                {printf("Palabra reservada %s\n", yytext);}
"integer"           {printf("Palabra reservada %s\n", yytext);}
"return"            {printf("Palabra reservada %s\n", yytext);}
"true"              {printf("Palabra reservada %s\n", yytext);}
"void"              {printf("Palabra reservada %s\n", yytext);}
"while"             {printf("Palabra reservada %s\n", yytext);}
"{"                 {printf("Llave abierta %s\n", yytext);}
"}"                 {printf("Llave cerrada %s\n", yytext);}
"("                 {printf("Parentesis abierto %s\n", yytext);}
")"                 {printf("Parentesis cerrado %s\n", yytext);}
";"                 {printf("Semicolon %s\n", yytext);}
","                 {printf("Coma %s\n", yytext);}
{ID}                {printf("Identificador %s\n", yytext);}
{INTEGER_LITERAL}   {printf("Literal %s\n", yytext);}
{BOOL_LITERAL}      {printf("Literal %s\n", yytext);}
"+"                 {printf("Operador %s\n", yytext);}
"*"                 {printf("Operador %s\n", yytext);}
"/"                 {printf("Operador %s\n", yytext);}
"-"                 {printf("Operador %s\n", yytext);}
"%"                 {printf("Operador %s\n", yytext);}
"=="                {printf("Operador %s\n", yytext);}
"&&"                {printf("Operador %s\n", yytext);}
"||"                {printf("Operador %s\n", yytext);}
"<"                 {printf("Operador %s\n", yytext);}
">"                 {printf("Operador %s\n", yytext);}
"="                 {printf("Operador %s\n", yytext);}
"!"                 {printf("Operador %s\n", yytext);}
{WHITE_SPACE}       {}
.                   {printf("Error: caracter no reconocido '%s'\n", yytext);}

%%

void yyerror(){
    printf("Linea %d Col %d\n└──Error de sintaxis. \n", yylineno, yycolumn);
}
