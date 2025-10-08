#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pre_asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones);
void directivas(FILE *out_s);
void decl_fun(FILE *out_s, Instrucciones *instrucciones);