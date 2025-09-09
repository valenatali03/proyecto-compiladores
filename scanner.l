%{
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.tab.h"
int yycolumn = 1;
%}

%option yylineno

ALPHA                           [a-zA-Z]
DIGIT                           [0-9]
ID                              {ALPHA}({ALPHA}|{DIGIT}|_)*
INTEGER_LITERAL                 {DIGIT}+
WHITE_SPACE                     [ \t\r\n]+

%%

"//".*                          {}           
"/*"([^*]|(\*+[^*/]))*"\*/"     {}
{WHITE_SPACE}                   {}
"extern"                        {printf("Palabra reservada %s\n", yytext); return T_EXTERN;}
"bool"                          {printf("Palabra reservada %s\n", yytext); yylval.tipo = BOOL; return T_BOOL;}
"program"                       {printf("Palabra reservada %s\n", yytext); return T_PROGRAM;}
"else"                          {printf("Palabra reservada %s\n", yytext); return T_ELSE;}
"then"                          {printf("Palabra reservada %s\n", yytext); return T_THEN;}
"false"                         {printf("Palabra reservada %s\n", yytext); yylval.b = false; return T_FALSE;}
"if"                            {printf("Palabra reservada %s\n", yytext); return T_IF;}
"integer"                       {printf("Palabra reservada %s\n", yytext); yylval.tipo = ENTERO; return T_INTEGER;}
"return"                        {printf("Palabra reservada %s\n", yytext); return T_RETURN;}
"true"                          {printf("Palabra reservada %s\n", yytext); yylval.b = true; return T_TRUE;}
"void"                          {printf("Palabra reservada %s\n", yytext); yylval.tipo = VACIO; return T_VOID;}
"while"                         {printf("Palabra reservada %s\n", yytext); return T_WHILE;}
"{"                             {printf("Llave abierta %s\n", yytext); return T_BO;}
"}"                             {printf("Llave cerrada %s\n", yytext); return T_BC;}
"("                             {printf("Parentesis abierto %s\n", yytext); return T_PO;}
")"                             {printf("Parentesis cerrado %s\n", yytext); return T_PC;}
";"                             {printf("Semicolon %s\n", yytext); return T_SCOLON;}
","                             {printf("Coma %s\n", yytext); return T_COMMA;}
"+"                             {printf("Operador %s\n", yytext); return T_ADD;}
"*"                             {printf("Operador %s\n", yytext); return T_MULT;}
"/"                             {printf("Operador %s\n", yytext); return T_DIV;}
"-"                             {printf("Operador %s\n", yytext); return T_MINUS;}
"%"                             {printf("Operador %s\n", yytext); return T_MOD;}
"=="                            {printf("Operador %s\n", yytext); return T_COMP;}
"&&"                            {printf("Operador %s\n", yytext); return T_AND;}
"||"                            {printf("Operador %s\n", yytext); return T_OR;}
"<"                             {printf("Operador %s\n", yytext); return T_LT;}
">"                             {printf("Operador %s\n", yytext); return T_GT;}
"="                             {printf("Operador %s\n", yytext); return T_EQ;}
"!"                             {printf("Operador %s\n", yytext); return T_NOT;}
{INTEGER_LITERAL}               {printf("Literal %s\n", yytext); yylval.num = atoi(yytext); return T_INT_LIT;}
{ID}                            {printf("Identificador %s\n", yytext); yylval.id = strdup(yytext); return T_ID;}
.                               {printf("Error: caracter no reconocido '%s'\n", yytext);}

%%

void yyerror(){
    printf("Linea %d Col %d\n└──Error de sintaxis. \n", yylineno, yycolumn);
}
