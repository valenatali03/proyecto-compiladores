#include "../includes/enums.h"

const char *tipo_str[] = {
    [VACIO] = "VACIO",
    [ENTERO] = "ENTERO",
    [BOOL] = "BOOL"};

const char *tipo_info_str[] = {
    [ID] = "ID",
    [OPERADOR_BINARIO] = "OPERADOR_BINARIO",
    [OPERADOR_UNARIO] = "OPERADOR_UNARIO",
    [LITERAL] = "LITERAL",
    [DECLARACION_VARIABLE] = "DECLARACION_VARIABLE",
    [ASIGNACION] = "ASIGNACION",
    [PROGRAMA] = "PROGRAMA",
    [SENTENCIAS] = "SENTENCIAS",
    [BLOQUE] = "BLOQUE",
    [RETURN] = "RETURN",
    [DECLARACIONES_VARIABLES] = "DECLARACIONES_VARIABLES",
    [DECLARACIONES_FUNCIONES] = "DECLARACIONES_FUNCIONES",
    [DECL_FUNCION] = "DECL_FUNCION",
    [CALL_FUNCION] = "CALL_FUNCION",
    [IF] = "IF",
    [WHILE] = "WHILE"};

const char *tipo_err_str[] = {
    [VAR_NO_DECLARADA] = "Variable no declarada",
    [VAR_YA_DECLARADA] = "Variable ya declarada",
    [VAR_VACIO] = "Variable vacía",
    [FUN_NO_DECLARADA] = "Función no declarada",
    [FUN_YA_DECLARADA] = "Función ya declarada",
    [TIPO_INCOMPATIBLE] = "Tipos incompatibles",
    [RETURN_TIPO_INCOMPATIBLE] = "Return con tipo incompatible",
    [CANT_PARAMETROS] = "Cantidad de parámetros incorrecta",
    [TIPO_PARAMETRO] = "Tipo de parámetro incorrecto",
    [FUN_SIN_RETURN] = "Función sin return",
    [MAIN_NO_DECLARADO] = "Función main no declarada",
    [MAIN_CON_PARAMS] = "Función main no debe recibir parámetros",
    [OP_BINARIO] = "Operación binaria inválida",
    [OP_UNARIO] = "Operación unaria inválida",
    [NUM_FUERA_RANGO] = "Número fuera de rango"};