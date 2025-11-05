#ifndef ERRORES_H
#define ERRORES_H

#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

#define MAX_ERRORES 100 // Máximo número de errores que se almacenan
#define MAX_WARNINGS 100 // Máximo número de warnings que se almacenan
#define MAX_MSG 250 // Longitud máxima del mensaje de error/warning

/**
 * Representa un error semántico detectado durante el análisis.
 */
typedef struct
{
    CodigoError codigo; // Código de error (enum definido en enums.h)
    char mensaje[MAX_MSG]; // Mensaje descriptivo del error
} Error;

/**
 * Representa un warning detectado durante las optimizaciones.
 */
typedef struct
{
    CodigoWarning codigo; // Código de warning (enum definido en enums.h)
    char mensaje[MAX_MSG]; // Mensaje descriptivo del warning
} Warning;

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
 * Reporta un warning detectado durante las optimizaciones del código.
 *
 * Construye y almacena un mensaje de advertencia basado en el 
 * código de warning (`CodigoWarning`) y los
 * parámetros adicionales provistos.
 *
 * Los warnings no detienen la ejecución del compilador, pero informan al
 * usuario sobre posibles problemas o ineficiencias, como variables o
 * funciones no utilizadas.
 *
 * @param codigo  Código de warning que indica el tipo de advertencia
 * @param linea   Línea del código fuente donde se detectó el warning
 * @param colum   Columna del código fuente donde se detectó el warning
 * @param ...     Argumentos adicionales según el tipo de warning
 */
void reportarWarning(CodigoWarning codigo, int linea, int colum, ...);

/**
 * Reporta el resultado del análisis semántico en un archivo de salida.
 *
 * Si no se detectaron errores, imprime un mensaje indicando que
 * el análisis fue exitoso. En caso contrario, imprime todos los errores
 * almacenados junto con sus descripciones.
 *
 */
int reportar_resultado_semantico();

/**
 * Reporta todos los warnings almacenados durante el proceso de compilación.
 * 
 * Esta función se invoca al finalizar las optimizaciones, para informar
 * al usuario sobre posibles mejoras o comportamientos sospechosos
 * detectados en el código.
 */
void reportar_warnings();

#endif
