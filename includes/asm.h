#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pre_asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones);
void directivas(FILE *out_s);
void operadores(FILE *out_s, Instrucciones *instrucciones);
void mov(FILE *out_s, Simbolo *arg1, Simbolo *res, bool inicio_fun);
void params_decl(Parametro_Decl *params, FILE *out_s, int cant_var, int cant_params);
void calcular_offset_var(Simbolo *var);