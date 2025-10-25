#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"
#include "tsim.h"

/**
 * Realiza el análisis semántico completo de un programa.
 *
 * @param arbol        Árbol de sintaxis abstracta (AST) raíz del programa.
 * @param tabla        Tabla de símbolos global.
 */
void analisis_semantico(Arbol *arbol, Nivel *tabla);

/**
 * Procesa las declaraciones de variables dentro de un nodo del AST.
 *
 * @param arbol        Nodo del AST que contiene la declaración.
 * @param tabla        Tabla de símbolos actual.
 */
void procesar_declaracion_variable(Arbol *arbol, Nivel *tabla);

/**
 * Declara una variable en la tabla de símbolos.
 *
 * @param arbol        Nodo del AST con la declaración de la variable.
 * @param tabla        Tabla de símbolos actual.
 */
void declarar_variable(Arbol *arbol, Nivel *tabla);

/**
 * Verifica si la inicialización de una variable global es constante.
 *
 * @param arbol  Nodo del AST de la expresión de inicialización.
 * @return       1 si la inicialización no es constante, 0 si es válida.
 */
int procesar_declaracion_variable_global(Arbol *arbol);

/**
 * Procesa las declaraciones de métodos (funciones).
 *
 * @param arbol        Nodo del AST con la declaración de métodos.
 * @param tabla        Tabla de símbolos actual.
 */
void procesar_declaracion_metodo(Arbol *arbol, Nivel *tabla);

/**
 * Declara un método en la tabla de símbolos.
 *
 * @param arbol        Nodo del AST con la declaración del método.
 * @param tabla        Tabla de símbolos actual.
 */
void declarar_metodo(Arbol *arbol, Nivel *tabla);


/**
 * Procesa y declara los parámetros formales de un método.
 *
 * @param params       Lista de parámetros de la declaración.
 * @param nivelActual  Nivel de la tabla de símbolos donde se insertan.
 */
void procesar_params(Parametro_Decl *params, Nivel *nivelActual);

/**
 * Procesa un bloque de código (crea un nuevo nivel de símbolos).
 *
 * @param arbol        Nodo del AST correspondiente al bloque.
 * @param nivelActual  Nivel actual de la tabla de símbolos.
 * @param params       Parámetros a declarar en este bloque (si existen).
 * @return             Nivel de la tabla tras cerrar el bloque.
 */
Nivel *procesar_bloque(Arbol *arbol, Nivel *nivelActual, Parametro_Decl *params);

/**
 * Procesa una lista de sentencias.
 *
 * @param arbol        Nodo del AST con las sentencias.
 * @param tabla        Tabla de símbolos actual.
 */
void procesar_statements(Arbol *arbol, Nivel *tabla);

/**
 * Procesa una estructura condicional IF.
 *
 * @param arbol        Nodo del AST que representa el IF.
 * @param nivelActual  Nivel de la tabla de símbolos.
 * @return             1 si el IF es válido semánticamente, 0 en caso contrario.
 */
int procesar_if(Arbol *arbol, Nivel *nivelActual);


/**
 * Procesa una estructura WHILE.
 *
 * @param arbol        Nodo del AST que representa el WHILE.
 * @param nivelActual  Nivel de la tabla de símbolos.
 * @return             1 si el WHILE es válido semánticamente, 0 en caso contrario.
 */
int procesar_while(Arbol *arbol, Nivel *nivelActual);

/**
 * Procesa una sentencia genérica.
 *
 * @param arbol        Nodo del AST con la sentencia.
 * @param tabla        Tabla de símbolos actual.
 */
void procesar_statement(Arbol *arbol, Nivel *tabla);

/**
 * Procesa una asignación de variable.
 *
 * @param arbol        Nodo del AST con la asignación.
 * @param tabla        Tabla de símbolos actual.
 * @return             1 si la asignación es válida, 0 en caso contrario.
 */
int procesar_asignacion(Arbol *arbol, Nivel *tabla);

/**
 * Procesa una expresión.
 *
 * @param arbol        Nodo del AST con la expresión.
 * @param tabla        Tabla de símbolos actual.
 * @return             1 si la expresión es válida, 0 en caso contrario.
 */
int procesar_expresion(Arbol *arbol, Nivel *tabla);

/**
 * Procesa un operador (binario o unario).
 *
 * @param arbol        Nodo del AST con el operador.
 * @param tabla        Tabla de símbolos actual.
 * @return             1 si el operador es válido, 0 en caso contrario.
 */
int procesar_operador(Arbol *arbol, Nivel *tabla);

/**
 * Obtiene el tipo de un nodo del AST.
 *
 * @param arbol        Nodo del AST.
 * @param tabla        Tabla de símbolos actual.
 * @return             Tipo del nodo (ENTERO, BOOL, etc.).
 */
Tipo obtener_tipo(Arbol *arbol, Nivel *tabla);

/**
 * Procesa una sentencia RETURN.
 *
 * @param arbol        Nodo del AST con el RETURN.
 * @param nivelActual  Nivel de la tabla de símbolos actual.
 */
void procesar_return(Arbol *arbol, Nivel *nivelActual);

/**
 * Procesa la llamada a un método.
 *
 * @param arbol        Nodo del AST con la llamada.
 * @param nivelActual  Nivel de la tabla de símbolos actual.
 * @return             1 si la llamada es válida, 0 en caso contrario.
 */
int procesar_metodo(Arbol *arbol, Nivel *nivelActual);

/**
 * Busca si en un conjunto de sentencias existe un RETURN.
 *
 * @param sentencias   Nodo del AST con sentencias.
 * @return             1 si hay RETURN, 0 si no.
 */
int buscar_return(Arbol *sentencias);

/**
 * Procesa la declaración de la función main.
 *
 * @param nombre       Nombre de la función.
 * @param params       Lista de parámetros de la función main.
 */
void procesar_main(char *nombre, Parametro_Decl *params);

#endif