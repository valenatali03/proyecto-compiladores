#include "errores.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Arreglo de errores
Error errores[MAX_ERRORES];
int cantErrores = 0;

void reportarError(CodigoError codigo, int linea, int colum, ...)
{
    if (cantErrores >= MAX_ERRORES)
        return;

    errores[cantErrores].codigo = codigo;

    va_list args;
    va_start(args, colum);

    switch (codigo)
    {
    case VAR_NO_DECLARADA:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: variable '%s' no declarada",
                 linea, colum, nombre);
        break;
    }
    case VAR_YA_DECLARADA:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: variable '%s' ya declarada",
                 linea, colum, nombre);
        break;
    }
    case VAR_VACIO:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: variable '%s' declarada VOID",
                 linea, colum, nombre);
        break;
    }
    case VAR_GLOBAL_NO_CONST:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: Variable global %s no inicializada con valor constante",
                 linea, colum, nombre);
        break;
    }
    case FUN_NO_DECLARADA:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: función '%s' no declarada",
                 linea, colum, nombre);
        break;
    }
    case FUN_YA_DECLARADA:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: función '%s' ya declarada",
                 linea, colum, nombre);
        break;
    }
    case FUN_SIN_RETURN:
    {
        const char *nombre = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: función '%s' debe retornar valor y no lo hace",
                 linea, colum, nombre);
        break;
    }
    case TIPO_INCOMPATIBLE:
    {
        Tipo esperado = va_arg(args, Tipo);
        Tipo recibido = va_arg(args, Tipo);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: tipo incompatible, se esperaba '%s' pero se recibió '%s'",
                 linea, colum, tipo_str[esperado], tipo_str[recibido]);
        break;
    }
    case RETURN_TIPO_INCOMPATIBLE:
    {
        const char *funcion = va_arg(args, const char *);
        Tipo esperado = va_arg(args, Tipo);
        Tipo recibido = va_arg(args, Tipo);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: función '%s' el tipo de retorno esperado es '%s' pero se recibió '%s'",
                 linea, colum, funcion, tipo_str[esperado], tipo_str[recibido]);
        break;
    }
    case CANT_PARAMETROS:
    {
        const char *funcion = va_arg(args, const char *);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: llamada a función '%s' con cantidad de parámetros incorrectos",
                 linea, colum, funcion);
        break;
    }
    case TIPO_PARAMETRO:
    {
        const char *funcion = va_arg(args, const char *);
        Tipo esperado = va_arg(args, Tipo);
        Tipo recibido = va_arg(args, Tipo);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: en función '%s', tipo de parámetro incompatible: se esperaba '%s' pero se recibió '%s'",
                 linea, colum, funcion, tipo_str[esperado], tipo_str[recibido]);
        break;
    }
    case MAIN_NO_DECLARADO:
    {
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: Main no declarado",
                 linea, colum);
        break;
    }
    case MAIN_CON_PARAMS:
    {
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: Main declarado con parametros",
                 linea, colum);
        break;
    }
    case OP_BINARIO:
    {
        const char *operador = va_arg(args, const char *);
        Tipo arg1 = va_arg(args, Tipo);
        Tipo arg2 = va_arg(args, Tipo);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: Operador '%s' no se puede aplicar a operandos de tipo '%s' y '%s'",
                 linea, colum, operador, tipo_str[arg1], tipo_str[arg2]);
        break;
    }
    case OP_UNARIO:
    {
        const char *operador = va_arg(args, const char *);
        Tipo arg = va_arg(args, Tipo);
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: Operador '%s' no se puede aplicar a operando de tipo '%s'",
                 linea, colum, operador, tipo_str[arg]);
        break;
    }
    case NUM_FUERA_RANGO:
    {
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error: El número se encuentra fuera de rango",
                 linea, colum);
        break;
    }
    case DIV_POR_CERO:
    {
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                "Linea %d Col %d └── Error: Se encontró una división por cero.", linea, colum);
        break;
    }

    default:
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error semántico desconocido",
                 linea, colum);
        break;
    }

    va_end(args);
    cantErrores++;
}

int reportar_resultado_semantico(FILE *out_sem) {
    if (cantErrores == 0) {
        fprintf(out_sem, "Análisis semántico exitoso\n");
        return 0;
    } else {
        for (int i = 0; i < cantErrores; i++) {
            fprintf(out_sem, "Error: %s\n", tipo_err_str[errores[i].codigo]);
            fprintf(out_sem, "%s\n", errores[i].mensaje);
        }
        return 1;
    }
}
