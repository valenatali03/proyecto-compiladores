#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pre_asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones);
void directivas(FILE *out_s);
void operadores(FILE *out_s, Instrucciones *instrucciones);
void calcular_offset(Simbolo *arg1, Simbolo *arg2, Simbolo *res);