#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"
#include "tsim.h"
#include "errores.h"

/**
 * Recorre el árbol y recolecta todas las declaraciones de variables.
 *
 * @param arbol Árbol de sintaxis abstracta (AST)
 * @param tabla Nivel donde se almacenan las declaraciones
 */
void recolectarDeclaraciones(Arbol *arbol, Nivel *tabla);

/**
 * Realiza chequeos de tipos en el AST.
 *
 * @param arbol Árbol de sintaxis abstracta (AST)
 * @param tabla Tabla de símbolos con las declaraciones recolectadas
 */
void chequearTipos(Arbol *arbol, Nivel *tabla);

/**
 * Ejecuta el análisis semántico completo.
 *
 * Primero procesa la función principal, luego recolecta declaraciones
 * y finalmente chequea tipos en las expresiones.
 *
 * @param arbol Árbol de sintaxis abstracta (AST)
 * @param tabla Tabla de símbolos
 */
void analisisSemantico(Arbol *arbol, Nivel *tabla);


/**
 * Procesa un nodo identificador.
 *
 * - Si está en una declaración: lo agrega a la tabla de símbolos (si no existe).
 * - Si está en otro contexto: verifica que esté declarado.
 *
 * @param hijo      Nodo identificador a procesar
 * @param tipoPadre Tipo de nodo padre (ej. DECLARACION, ASIGNACION)
 * @param tabla     Nivel de la tabla de símbolos
 * @return 1 si el identificador es válido, 0 en caso de error
 */
int procesarId(Arbol *hijo, Tipo_Info tipoPadre, Nivel *tabla);

/**
 * Procesa una asignación, verificando la existencia y compatibilidad de tipos.
 *
 * @param arbol Nodo de asignación
 * @param tabla Tabla de símbolos
 */
void procesarAsignacion(Arbol *arbol, Simbolo *tabla);

/**
 * Procesa una declaración de variable.
 *
 * @param arbol Nodo de declaración
 * @param tabla Tabla de símbolos
 */
void procesarDeclaracion(Arbol *arbol, Simbolo *tabla);

/**
 * Procesa un operador, verificando tipos de operandos y propagando el tipo resultante.
 *
 * @param arbol Nodo operador
 * @param tabla Tabla de símbolos
 */
void procesarOperador(Arbol *arbol, Simbolo *tabla);

/**
 * Procesa la declaración de una función.
 *
 * @param arbol Nodo función
 * @param tabla Tabla de símbolos
 */
void procesarFuncion(Arbol *arbol, Simbolo *tabla);

/**
 * Procesa un retorno dentro de una función.
 *
 * Verifica que el tipo retornado sea compatible con el tipo de la función.
 *
 * @param arbol Nodo return
 * @param tabla Tabla de símbolos
 */
void procesarReturn(Arbol *arbol, Simbolo *tabla);

#endif