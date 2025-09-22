#ifndef AST_H
#define AST_H
#include "info.h"
#include "params.h"
#include "enums.h"
#include <stdio.h>

/**
 * Nodo del árbol de sintaxis abstracta (AST).
 */
typedef struct Arbol
{
    Info_Union *info;
    Tipo_Info tipo_info;
    int linea;
    int colum;
    struct Arbol *izq;
    struct Arbol *medio;
    struct Arbol *der;
} Arbol;

/**
 * Crea un nodo operador del AST.
 *
 * @param op     Nombre del operador
 * @param valor  Valor asociado al operador (si lo hubiera)
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param izq    Hijo izquierdo
 * @param der    Hijo derecho
 * @return       Puntero al nuevo nodo operador
 */
Arbol *crear_arbol_operador(char *op, Tipo_Info tipo_op, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der);

/**
 * Crea un nodo identificador del AST.
 *
 * @param id     Nombre del identificador
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param izq    Hijo izquierdo
 * @param der    Hijo derecho
 * @return       Puntero al nuevo nodo identificador
 */
Arbol *crear_arbol_id(char *id, int linea, int colum, Arbol *izq, Arbol *der);

/**
 * Crea un nodo literal del AST.
 *
 * @param valor  Valor del literal
 * @param tipo   Tipo del literal
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param izq    Hijo izquierdo
 * @param der    Hijo derecho
 * @return       Puntero al nuevo nodo literal
 */
Arbol *crear_arbol_literal(void *valor, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der);

/**
 * Crea un nodo de declaración de función del AST.
 *
 * @param nombre Nombre de la función
 * @param tipo   Tipo de retorno de la función
 * @param parametros Parametros de la función
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param izq    Hijo izquierdo
 * @param der    Hijo derecho
 * @return       Puntero al nuevo nodo función
 */
Arbol *crear_arbol_funcion_decl(char *nombre, Tipo tipo, Parametro_Decl *params, int linea, int colum, Arbol *izq, Arbol *der);

/**
 * Crea un nodo de llamada de función del AST.
 *
 * @param nombre Nombre de la función
 * @param parametros Parametros de la función
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param izq    Hijo izquierdo
 * @param der    Hijo derecho
 * @return       Puntero al nuevo nodo función
 */
Arbol *crear_arbol_funcion_call(char *nombre, Parametro_Call *params, int linea, int colum, Arbol *izq, Arbol *der);

/**
 * Crea un nodo IF del AST.
 *
 * @param tipo   Tipo de nodo
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param medio  Subárbol que representa la condición del IF
 * @param izq    Subárbol correspondiente a la rama "then"
 * @param der    Subárbol correspondiente a la rama "else"
 * @return       Puntero al nuevo nodo IF
 */
Arbol *crear_arbol_if(Tipo_Info tipo, int linea, int colum, Arbol *izq, Arbol *medio, Arbol *der);

/**
 * Crea un nodo genérico del AST.
 *
 * @param tipo   Tipo de nodo (ver @ref Tipo_Info)
 * @param linea  Línea del código fuente
 * @param colum  Columna del código fuente
 * @param izq    Hijo izquierdo
 * @param der    Hijo derecho
 * @return       Puntero al nuevo nodo
 */
Arbol *crear_arbol_nodo(Tipo_Info tipo, int linea, int colum, Arbol *izq, Arbol *der);

/**
 * Función recursiva que imprime un nodo del AST en formato DOT
 *
 * @param arbol Puntero al nodo del AST a imprimir
 * @param f     Archivo DOT abierto donde se escribirá la información
 */
void imprimir_nodo_dot(Arbol *arbol, FILE *f);

/**
 * Genera un archivo DOT a partir de un AST completo.
 *
 * @param raiz           Puntero a la raíz del AST
 * @param nombre_archivo Nombre del archivo DOT a generar
 *
 * La función abre el archivo, escribe la cabecera del grafo,
 * recorre todos los nodos del AST mediante `imprimir_nodo_dot`,
 * y finalmente cierra el archivo DOT.
 */
void exportar_ast_a_dot(Arbol *raiz, const char *nombre_archivo);

#endif