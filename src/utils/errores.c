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
    default:
        snprintf(errores[cantErrores].mensaje, MAX_MSG,
                 "Linea %d Col %d\n└── Error semántico desconocido",
                 linea, colum);
        break;
    }

    va_end(args);
    cantErrores++;
}

void mostrarErrores(void)
{
    for (int i = 0; i < cantErrores; i++)
    {
        printf("%s\n", errores[i].mensaje);
    }
}
