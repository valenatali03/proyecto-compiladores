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
    OPERADOR_BINARIO,
    OPERADOR_UNARIO,
    LITERAL,
    DECLARACION_VARIABLE,
    ASIGNACION,
    PROGRAMA,
    SENTENCIAS,
    BLOQUE,
    RETURN,
    DECLARACIONES_VARIABLES,
    DECLARACIONES_FUNCIONES,
    DECL_FUNCION,
    CALL_FUNCION,
    IF,
    WHILE
} Tipo_Info;

// Tipos de errores
typedef enum
{
    VAR_NO_DECLARADA,
    VAR_YA_DECLARADA,
    VAR_VACIO,
    FUN_NO_DECLARADA,
    FUN_YA_DECLARADA,
    TIPO_INCOMPATIBLE,
    RETURN_TIPO_INCOMPATIBLE,
    CANT_PARAMETROS,
    TIPO_PARAMETRO,
    FUN_SIN_RETURN,
    MAIN_NO_DECLARADO,
    MAIN_CON_PARAMS,
    OP_BINARIO,
    OP_UNARIO,
    NUM_FUERA_RANGO
} CodigoError;

extern const char *tipo_str[];
extern const char *tipo_info_str[];
extern const char *tipo_err_str[];

#endif