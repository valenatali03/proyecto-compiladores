#ifndef ERRORES_H
#define ERRORES_H

#include <stdbool.h>
#include "enums.h"

#define MAX_ERRORES 100
#define MAX_MSG 250

typedef enum
{
    VAR_NO_DECLARADA,
    VAR_YA_DECLARADA,
    VAR_VACIO,
    FUN_NO_DECLARADA,
    FUN_YA_DECLARADA,
    TIPO_INCOMPATIBLE,
    CANT_PARAMETROS,
    TIPO_PARAMETRO,
    FUN_SIN_RETURN,
    MAIN_NO_DECLARADO,
    MAIN_CON_PARAMS
} CodigoError;

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
void mostrarErrores(void);

#endif
