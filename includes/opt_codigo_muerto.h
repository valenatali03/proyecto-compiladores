#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "errores.h"

/**
 * Aplica optimizaciones de código muerto sobre el AST completo.
 *
 * @param arbol Puntero al nodo raíz del AST
 */
void opt_codigo_muerto(Arbol *arbol);

/**
 * Elimina nodos que nunca se ejecutarán después de un RETURN dentro de un bloque de sentencias.
 *
 * @param arbol Nodo raíz de la subrama de sentencias
 */
void opt_post_return(Arbol *arbol); 

/**
 * Determina si un subárbol contiene un nodo RETURN.
 *
 * @param arbol Nodo raíz del subárbol
 * @return 1 si existe un RETURN, 0 en caso contrario
 */
int existe_return(Arbol *arbol);

/**
 * Optimiza los nodos de control de flujo (`IF` y `WHILE`) en todo el AST.
 *
 * @param arbol Nodo raíz del AST
 */
void opt_flujo_control(Arbol *arbol);

/**
 * Optimiza un único nodo de control de flujo (`IF` o `WHILE`).
 *
 * @param subarbol Nodo a optimizar
 * @return Nodo optimizado, puede ser NULL si se eliminó
 */
Arbol *opt_nodo_control(Arbol *subarbol);

/**
 * Optimiza un nodo IF evaluando su condición literal y eliminando ramas muertas.
 *
 * @param arbol Nodo IF a optimizar
 * @return Nodo optimizado, que puede ser NULL si la rama se elimina
 */
Arbol *optimizar_if(Arbol *arbol);

/**
 * Optimiza un nodo WHILE evaluando su condición literal y eliminando bucles muertos.
 *
 * @param arbol Nodo WHILE a optimizar
 * @return Nodo optimizado, que puede ser NULL si el bucle se elimina
 */
Arbol *optimizar_while(Arbol *arbol);

/**
 * Optimiza las declaraciones del AST eliminando variables y funciones no usadas.
 * 
 * - Si una variable o función nunca se usa, se libera y se reporta un warning.
 * - Si un nodo agrupa declaraciones y queda vacío, se elimina.
 * - Si solo tiene un hijo útil del mismo tipo, se reemplaza por el hijo.
 *
 * @param arbol Nodo raíz del subárbol de declaraciones
 * @return Nodo optimizado, o NULL si fue eliminado
 */
Arbol *opt_declaraciones(Arbol *arbol);

/**
 * Libera recursivamente un nodo del AST y todos sus hijos.
 *
 * @param arbol Nodo a liberar
 */
void liberar_nodo(Arbol *arbol);