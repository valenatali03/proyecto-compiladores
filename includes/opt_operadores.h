#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errores.h"
#include "info.h"

/**
 * Función principal que recorre el AST buscando nodos que puedan ser optimizados.
 * Llama a 'reducir_operador' en subárboles de operación y
 * a 'optimizar_parametros' en llamadas a función.
 *
 * @param arbol El nodo del AST desde el cual comenzar el recorrido.
 */
void optimizar_operaciones(Arbol *arbol);

/**
 * Intenta reducir un nodo de operación (binario o unario).
 * Si sus operandos son literales, evalúa la operación, 
 * crea un nuevo nodo LITERAL con el resultado,
 * libera los nodos antiguos (operador y operandos) y retorna el nuevo nodo literal.
 * Si no puede reducirse, retorna el nodo original.
 *
 * @param nodo El nodo de operación (OPERADOR_BINARIO, OPERADOR_UNARIO) a reducir.
 * @return     El nodo resultante (un nuevo LITERAL si fue exitoso, o el 'nodo' original).
 */
Arbol *reducir_operador(Arbol *nodo);

/**
 * Función auxiliar que ejecuta la operación binaria especificada entre dos literales.
 * Reserva nueva memoria ('malloc') para el valor resultante (ej. int* o bool*).
 *
 * @param a             Puntero a la información del literal izquierdo.
 * @param b             Puntero a la información del literal derecho.
 * @param nodo_operador El nodo del operador (usado para info de tipo y operador).
 * @return              Un puntero 'void*' al valor resultante (en nueva memoria), o NULL si la op no es soportada.
 */
void *realizar_operacion_binaria(Info_Literal *a, Info_Literal *b, Arbol *nodo_operador);

/**
 * Función auxiliar que ejecuta la operación unaria especificada sobre un literal.
 * Reserva nueva memoria ('malloc') para el valor resultante.
 *
 * @param a             Puntero a la información del literal.
 * @param nodo_operador El nodo del operador (usado para info de tipo y operador).
 * @return              Un puntero 'void*' al valor resultante (en nueva memoria), o NULL si la op no es soportada.
 */
void *realizar_operacion_unaria(Info_Literal *a, Arbol *nodo_operador);

/**
 * Libera la memoria de un nodo del AST.
 * 
 * @param nodo El nodo del árbol a liberar.
 */
void liberar_nodo_arbol(Arbol *nodo);

/**
 * Optimiza las expresiones usadas como parámetros en una llamada a función.
 * Itera la lista enlazada 'Parametro_Call' del nodo 'CALL_FUNCION'
 * y aplica 'reducir_operador' a cada expresión.
 *
 * @param nodo Un nodo del AST de tipo CALL_FUNCION.
 */
void optimizar_parametros(Arbol* nodo);




