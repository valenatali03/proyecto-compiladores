#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pre_asm.h"

void cargar_a_registro(FILE *out_s, Simbolo *sym, const char *reg);
void guardar_desde_registro(FILE *out_s, const char *reg, Simbolo *dest);
void aplicar_operacion_binaria(FILE *out_s, Simbolo *arg2, const char *instruccion);
void generar_asm(FILE *out_s, Instrucciones *instrucciones);
void directivas(FILE *out_s);
void operadores(FILE *out_s, Instrucciones *instrucciones);
void mov(FILE *out_s, Simbolo *arg1, Simbolo *res, bool inicio_fun);
void params_decl(Parametro_Decl *params, FILE *out_s, int cant_var, int cant_params);
void calcular_offset_var(Simbolo *var);