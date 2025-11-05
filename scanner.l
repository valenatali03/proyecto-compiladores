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

%%

"//".*                          {}           
"/*"([^*]|(\*+[^*/]))*"\*/"     {}
[ ]+        										{ yycolumn += yyleng; }
\t+         										{ yycolumn += 4 * yyleng; }
\n          										{ yycolumn = 1; }
\r          										{}
"extern"                        {yycolumn += yyleng; return T_EXTERN;}
"bool"                          {yycolumn += yyleng; yylval.tipo = BOOL; return T_BOOL;}
"program"                       {yycolumn += yyleng; return T_PROGRAM;}
"else"                          {yycolumn += yyleng; return T_ELSE;}
"then"                          {yycolumn += yyleng; return T_THEN;}
"false"                         {yycolumn += yyleng; yylval.b = false; return T_FALSE;}
"if"                            {yycolumn += yyleng; return T_IF;}
"integer"                       {yycolumn += yyleng; yylval.tipo = ENTERO; return T_INTEGER;}
"return"                        {yycolumn += yyleng; return T_RETURN;}
"true"                          {yycolumn += yyleng; yylval.b = true; return T_TRUE;}
"void"                          {yycolumn += yyleng; yylval.tipo = VACIO; return T_VOID;}
"while"                         {yycolumn += yyleng; return T_WHILE;}
"{"                             {yycolumn += yyleng; return T_BO;}
"}"                             {yycolumn += yyleng; return T_BC;}
"("                             {yycolumn += yyleng; return T_PO;}
")"                             {yycolumn += yyleng; return T_PC;}
";"                             {yycolumn += yyleng; return T_SCOLON;}
","                             {yycolumn += yyleng; return T_COMMA;}
"+"                             {yycolumn += yyleng; return T_ADD;}
"*"                             {yycolumn += yyleng; return T_MULT;}
"/"                             {yycolumn += yyleng; return T_DIV;}
"-"                             {yycolumn += yyleng; return T_MINUS;}
"%"                             {yycolumn += yyleng; return T_MOD;}
"=="                            {yycolumn += yyleng; return T_COMP;}
"&&"                            {yycolumn += yyleng; return T_AND;}
"||"                            {yycolumn += yyleng; return T_OR;}
"<"                             {yycolumn += yyleng; return T_LT;}
">"                             {yycolumn += yyleng; return T_GT;}
"="                             {yycolumn += yyleng; return T_EQ;}
"!"                             {yycolumn += yyleng; return T_NOT;}
{INTEGER_LITERAL}               {yycolumn += yyleng; yylval.num = atoi(yytext); return T_INT_LIT;}
{ID}                            {yycolumn += yyleng; yylval.id = strdup(yytext); return T_ID;}
.                               {yycolumn += yyleng; printf("Error: caracter no reconocido '%s'\n", yytext);}

%%
