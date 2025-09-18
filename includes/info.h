#ifndef INFO_H
#define INFO_H

#include "enums.h"

typedef struct Parametro_Decl Parametro_Decl;
typedef struct Parametro_Call Parametro_Call;

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
typedef struct Info_FuncionDecl
{
    char *nombre;
    void *valor;
    Tipo tipo;
    Parametro_Decl *params;
} Info_FuncionDecl;

typedef struct Info_FuncionCall
{
    char *nombre;
    Parametro_Call *params;
} Info_FuncionCall;

/**
 * Unión que agrupa la información posible en un nodo del árbol.
 */
typedef struct Info_Union
{
    Info_ID id;
    Info_Operador operador;
    Info_Literal literal;
    Info_FuncionDecl funcion_decl;
    Info_FuncionCall funcion_call;
} Info_Union;

#endif