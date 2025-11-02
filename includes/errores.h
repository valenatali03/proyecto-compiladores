#ifndef ERRORES_H
#define ERRORES_H

#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

#define MAX_ERRORES 100 // Máximo número de errores que se almacenan
#define MAX_MSG 250 // Longitud máxima del mensaje de error

/**
 * Representa un error semántico detectado durante el análisis.
 */
typedef struct
{
    CodigoError codigo; // Código de error (enum definido en enums.h)
    char mensaje[MAX_MSG]; // Mensaje descriptivo del error
} Error;

// Declaración de variables globales

/**
 * Arreglo global donde se almacenan los errores detectados.
 */
extern Error errores[MAX_ERRORES];

/**
 * Cantidad de errores almacenados actualmente en el arreglo.
 */
extern int cantErrores;

/**
 * Reporta un error semántico detectado.
 *
 * Dependiendo del tipo de error (`CodigoError`), la función recibe
 * parámetros adicionales que permiten construir un mensaje de error
 * detallado
 *
 * @param codigo  Código de error que indica el tipo de problema
 * @param linea   Línea del código fuente donde ocurre el error
 * @param colum   Columna del código fuente donde ocurre el error
 * @param ...     Argumentos adicionales según el tipo de error
 */
void reportarError(CodigoError codigo, int linea, int colum, ...);

/**
 * Reporta el resultado del análisis semántico en un archivo de salida.
 *
 * Si no se detectaron errores, imprime un mensaje indicando que
 * el análisis fue exitoso. En caso contrario, imprime todos los errores
 * almacenados junto con sus descripciones.
 *
 */
int reportar_resultado_semantico();

#endif
