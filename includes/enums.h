#ifndef ENUMS_H
#define ENUMS_H

// Tipos de datos posibles
typedef enum
{
    VACIO,
    ENTERO,
    BOOL
} Tipo;

// Tipos de nodos en el AST
typedef enum
{
    ID,
    OPERADOR,
    LITERAL,
    DECLARACION,
    ASIGNACION,
    PROGRAMA,
    SENTENCIAS,
    RETURN,
    DECLARACIONES,
    FUNCION_DECL,
    FUNCION_CALL,
    IF,
    WHILE
} Tipo_Info;

#endif