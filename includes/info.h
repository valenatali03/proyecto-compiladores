#ifndef INFO_H
#define INFO_H

#include "enums.h"

/**
 * Información asociada a un identificador en el árbol.
 */
typedef struct Info_ID
{
    char *nombre;
    void *valor;
    Tipo tipo;
} Info_ID;

/**
 * Información asociada a un operador en el árbol.
 */
typedef struct Info_Operador
{
    char *nombre;
    void *valor;
    Tipo tipo;
} Info_Operador;

/**
 * Información asociada a un literal (entero, booleano, etc.).
 */
typedef struct Info_Literal
{
    void *valor;
    Tipo tipo;
} Info_Literal;

/**
 * Información asociada a una función.
 */
typedef struct Info_Funcion
{
    char *nombre;
    void *valor;
    Tipo tipo;
} Info_Funcion;

/**
 * Unión que agrupa la información posible en un nodo del árbol.
 */
typedef struct Info_Union
{
    Info_ID id;
    Info_Operador operador;
    Info_Literal literal;
    Info_Funcion funcion;
} Info_Union;

#endif