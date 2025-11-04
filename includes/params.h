#ifndef PARAMS_H
#define PARAMS_H

#include "enums.h"
#include "info.h"

typedef struct Info_Union Info_Union;
typedef struct Arbol Arbol;

/**
 * Estructura de un nodo en la lista enlazada de parámetros en la declaración de una función.
 *
 * - info:  Información del parámetro.
 * - next:  Puntero al siguiente parámetro en la lista.
 * - linea: Número de línea donde se declara el parámetro.
 * - colum: Número de columna donde se declara el parámetro.
 */
typedef struct Parametro_Decl
{
    Info_Union *info;
    struct Parametro_Decl *next;
    int linea;
    int colum;
} Parametro_Decl;

/**
 * Estructura de un nodo en la lista enlazada de argumentos en una llamada a función.
 *
 * - expr:  Puntero al subárbol (AST) de la expresión pasada como argumento.
 * - next:  Puntero al siguiente argumento en la lista.
 * - linea: Número de línea donde se pasa el argumento.
 * - colum: Número de columna donde se pasa el argumento.
 */
typedef struct Parametro_Call
{
    Arbol *expr;
    struct Parametro_Call *next;
    int linea;
    int colum;
} Parametro_Call;

/**
 * Agrega un nuevo parámetro (de declaración o de llamada) al final de una
 * lista enlazada existente.
 *
 * @param params      Puntero a la cabeza de la lista de parámetros.
 * @param data        Puntero al dato a agregar.
 * @param tipo_param  Flag (DECL_FUNCION o CALL_FUNCION) que indica el tipo de lista/dato.
 * @param linea       Número de línea para el nuevo nodo.
 * @param colum       Número de columna para el nuevo nodo.
 * @return            Puntero a la cabeza de la lista.
 */
void *agregar_param(void *params, void *data, Tipo_Info tipo_param, int linea, int colum);

/**
 * Cuenta el número de parámetros en una lista de declaración.
 *
 * @param params Puntero a la cabeza de la lista de declaración.
 * @return       El número total de parámetros.
 */
int contar_parametros_decl(Parametro_Decl *params);

/**
 * Cuenta el número de argumentos en una lista de invocación.
 *
 * @param params Puntero a la cabeza de la lista de invocación.
 * @return       El número total de argumentos.
 */
int contar_parametros_call(Parametro_Call *params);

#endif