#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pre_asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones);
void directivas(FILE *out_s);
void operadores(FILE *out_s, Instrucciones *instrucciones);
void calcular_offset_var(Simbolo *var);
void calcular_offset_param_reg(Parametro_Decl *param, int cant_var);
void calcular_offset_param(Parametro_Decl *param);
void params_decl(Parametro_Decl *params, FILE *out_s, int cant_var, int cant_params);