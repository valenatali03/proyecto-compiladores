#ifndef ERRORES_H
#define ERRORES_H

#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

#define MAX_ERRORES 100
#define MAX_MSG 250

typedef struct
{
    CodigoError codigo;
    char mensaje[MAX_MSG];
} Error;

// Declaración del arreglo global
extern Error errores[MAX_ERRORES];
extern int cantErrores;

// Funciones
void reportarError(CodigoError codigo, int linea, int colum, ...);

void reportar_resultado_semantico(FILE *out_sem);

#endif
