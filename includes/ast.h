#ifndef AST
#define AST
#include "info.h"

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
Arbol *crear_arbol_operador(char *op, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der);

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
 * Crea un nodo función del AST.
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

Arbol *crear_arbol_funcion_call(char *nombre, Parametro_Call *params, int linea, int colum, Arbol *izq, Arbol *der);

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
 * Recorre el árbol en orden e imprime los nodos.
 *
 * @param arbol Raíz del árbol a recorrer
 */
void inorder(Arbol *arbol);

/**
 * Imprime el árbol en forma vertical (formato de árbol).
 *
 * @param arbol     Raíz del árbol a imprimir
 * @param prefijo   Prefijo usado para alinear ramas
 * @param es_ultimo Indica si el nodo es el último hijo
 */
void imprimir_vertical(Arbol *arbol, char *prefijo, int es_ultimo);

#endif