#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pre_asm.h"

/**
 * Carga el valor de un símbolo en un registro específico.
 *
 * @param out_s  Archivo de salida donde se escriben las instrucciones assembly.
 * @param sym    Símbolo cuyo valor se cargará.
 * @param reg    Registro destino donde se cargará el valor.
 */
void cargar_a_registro(FILE *out_s, Simbolo *sym, const char *reg);

/**
 * Guarda el valor de un registro en la ubicación de un símbolo destino.
 *
 * @param out_s  Archivo de salida donde se escriben las instrucciones assembly.
 * @param reg    Registro fuente que contiene el valor a guardar.
 * @param dest   Símbolo destino donde se almacenará el valor del registro.
 */
void guardar_desde_registro(FILE *out_s, const char *reg, Simbolo *dest);

/**
 * Aplica una operación binaria entre el valor en el registro %eax y otro símbolo.
 *
 * @param out_s       Archivo de salida donde se escriben las instrucciones assembly.
 * @param arg2        Segundo argumento de la operación.
 * @param instruccion Instrucción assembly que representa la operación binaria.
 */
void aplicar_operacion_binaria(FILE *out_s, Simbolo *arg2, const char *instruccion);

/**
 * Genera el código assembly a partir de una lista de instrucciones intermedias.
 *
 * @param out_s        Archivo de salida donde se escriben las instrucciones assembly.
 * @param instrucciones Lista de instrucciones intermedias a convertir.
 */
void generar_asm(FILE *out_s, Instrucciones *instrucciones);

void directivas(FILE *out_s);

/**
 * Genera instrucciones assembly para operaciones aritméticas y lógicas.
 *
 * @param out_s        Archivo de salida donde se escriben las instrucciones assembly.
 * @param instrucciones Instrucciones que contienen la operación a procesar.
 */
void operadores(FILE *out_s, Instrucciones *instrucciones);

/**
 * Mueve el valor de un símbolo a otro, generando la instrucción assembly correspondiente.
 *
 * @param out_s     Archivo de salida donde se escriben las instrucciones assembly.
 * @param arg1      Símbolo origen del valor.
 * @param res       Símbolo destino donde se almacenará el valor.
 * @param inicio_fun Indica si la instrucción se encuentra al inicio de una función.
 */
void mov(FILE *out_s, Simbolo *arg1, Simbolo *res, bool inicio_fun);

/**
 * Declara los parámetros de una función en su stack frame correspondiente.
 *
 * @param params      Lista de parámetros de la función.
 * @param out_s       Archivo de salida donde se escriben las instrucciones assembly.
 * @param cant_var    Cantidad total de variables locales de la función.
 * @param cant_params Cantidad de parámetros de la función.
 */
void params_decl(Parametro_Decl *params, FILE *out_s, int cant_var, int cant_params);

/**
 * Calcula y asigna el offset de una variable en el stack frame si aún no tiene uno.
 *
 * @param var Símbolo que representa la variable a calcular su offset.
 */
void calcular_offset_var(Simbolo *var);

void obtener_representacion_operando(Simbolo *sym, char *buffer, size_t size);