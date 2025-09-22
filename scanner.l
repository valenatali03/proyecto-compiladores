%{
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.tab.h"
int yycolumn = 1;

extern FILE *out_lex; 
%}

%option yylineno

ALPHA                           [a-zA-Z]
DIGIT                           [0-9]
ID                              {ALPHA}({ALPHA}|{DIGIT}|_)*
INTEGER_LITERAL                 {DIGIT}+

%%

"//".*                          {}           
"/*"([^*]|(\*+[^*/]))*"\*/"     {}
[ ]+        										{ yycolumn += yyleng; }
\t+         										{ yycolumn += 4 * yyleng; }
\n          										{ yycolumn = 1; }
\r          										{}
"extern"                        {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_EXTERN;}
"bool"                          {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); yylval.tipo = BOOL; return T_BOOL;}
"program"                       {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_PROGRAM;}
"else"                          {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_ELSE;}
"then"                          {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_THEN;}
"false"                         {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); yylval.b = false; return T_FALSE;}
"if"                            {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_IF;}
"integer"                       {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); yylval.tipo = ENTERO; return T_INTEGER;}
"return"                        {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_RETURN;}
"true"                          {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); yylval.b = true; return T_TRUE;}
"void"                          {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); yylval.tipo = VACIO; return T_VOID;}
"while"                         {yycolumn += yyleng; fprintf(out_lex,"Palabra reservada %s\n", yytext); return T_WHILE;}
"{"                             {yycolumn += yyleng; fprintf(out_lex,"Llave abierta %s\n", yytext); return T_BO;}
"}"                             {yycolumn += yyleng; fprintf(out_lex,"Llave cerrada %s\n", yytext); return T_BC;}
"("                             {yycolumn += yyleng; fprintf(out_lex,"Parentesis abierto %s\n", yytext); return T_PO;}
")"                             {yycolumn += yyleng; fprintf(out_lex,"Parentesis cerrado %s\n", yytext); return T_PC;}
";"                             {yycolumn += yyleng; fprintf(out_lex,"Semicolon %s\n", yytext); return T_SCOLON;}
","                             {yycolumn += yyleng; fprintf(out_lex,"Coma %s\n", yytext); return T_COMMA;}
"+"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_ADD;}
"*"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_MULT;}
"/"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_DIV;}
"-"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_MINUS;}
"%"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_MOD;}
"=="                            {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_COMP;}
"&&"                            {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_AND;}
"||"                            {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_OR;}
"<"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_LT;}
">"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_GT;}
"="                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_EQ;}
"!"                             {yycolumn += yyleng; fprintf(out_lex,"Operador %s\n", yytext); return T_NOT;}
{INTEGER_LITERAL}               {yycolumn += yyleng; fprintf(out_lex,"Literal %s\n", yytext); yylval.num = atoi(yytext); return T_INT_LIT;}
{ID}                            {yycolumn += yyleng; fprintf(out_lex,"Identificador %s\n", yytext); yylval.id = strdup(yytext); return T_ID;}
.                               {yycolumn += yyleng; fprintf(out_lex,"Error: caracter no reconocido '%s'\n", yytext);}

%%
