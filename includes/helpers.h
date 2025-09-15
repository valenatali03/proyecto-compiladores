#ifndef HELPERS_H
#define HELPERS_H
#include "enums.h"
#include "ast.h"

/**
 * Imprime un valor según su tipo con un prefijo opcional.
 *
 * - ENTERO → imprime un número entero.
 * - BOOL → imprime true/false.
 * - Otro → imprime tipo desconocido.
 *
 * @param tipo Tipo de dato del valor
 * @param valor Puntero al valor (ej: int*, bool*)
 * @param prefix Prefijo a mostrar antes del valor (ej: "[DEBUG] ")
 */
void print_valor(Tipo tipo, void *valor, const char *prefix);

/**
 * Obtiene el tipo de dato asociado a un nodo del AST.
 *
 * @param nodo Nodo del AST
 * @return Tipo del nodo o VACIO si no aplica
 */
Tipo obtenerTipoNodo(Arbol *nodo);

/**
 * Obtiene el valor almacenado en un nodo del AST.
 *
 * Según el tipo del nodo devuelve:
 * - literal.valor
 * - operador.valor
 * - id.valor
 *
 * @param nodo Nodo del AST
 * @return Puntero al valor o NULL si no corresponde
 */
void *obtener_valor(Arbol *nodo);

/**
 * Calcula el resultado de un operador aplicado a dos valores.
 *
 * Operadores soportados:
 * - Aritméticos: `+`, `*`
 * - Lógicos: `&&`, `||`, `!`, `==`
 *
 * @param op Cadena con el operador (ej: "+", "&&")
 * @param valor_izq Puntero al valor izquierdo
 * @param valor_der Puntero al valor derecho (NULL en operadores unarios)
 * @param tipo Tipo de los operandos
 * @return Puntero al valor resultante (reservado con malloc)
 */
void *calcular_op(char *op, void *valor_izq, void *valor_der, Tipo tipo);

#endif