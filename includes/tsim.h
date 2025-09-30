#ifndef TSIM_H
#define TSIM_H

#include "ast.h"

// Contador global de niveles (ámbitos) abiertos
extern int CANT_NIVELES;

/**
 * Estructura de un símbolo en la tabla de símbolos.
 * - info: puntero a la información asociada al símbolo (variable, función)
 * - next: puntero al siguiente símbolo en la lista del mismo nivel
 * - flag: indica el tipo de información almacenada (ID, DECL_FUNCION)
 */
typedef struct Simbolo
{
    Info_Union *info;
    struct Simbolo *next;
    Tipo_Info flag;
} Simbolo;
/**
 * Estructura de un nivel (ámbito) de la tabla de símbolos.
 * - head: puntero al primer símbolo de la lista en este nivel
 * - parent: puntero al nivel padre (ámbito superior)
 */
typedef struct Nivel
{
    struct Simbolo *head;
    struct Nivel *parent;
} Nivel;

/**
 * Crea la tabla de símbolos inicial (nivel raíz).
 *
 * @return Puntero al nivel raíz recién creado
 */
Nivel *crear_tabla();

/**
 * Abre un nuevo nivel (ámbito) dentro de la tabla de símbolos.
 *
 * @param tabla Nivel actual al que se agregará el nuevo nivel
 * @return Puntero al nuevo nivel creado
 */
Nivel *abrir_nivel(Nivel *tabla);

/**
 * Cierra el nivel actual y retorna el nivel padre.
 *
 * @param tabla Nivel actual a cerrar
 * @return Puntero al nivel padre
 */
Nivel *cerrar_nivel(Nivel *tabla);

/**
 * Agrega un símbolo a un nivel específico.
 *
 * @param nivel Nivel donde se agregará el símbolo
 * @param info  Información asociada al símbolo
 * @param flag  Tipo de información (ID, DECL_FUNCION)
 */
void agregar_simbolo(Nivel *nivel, Info_Union *info, Tipo_Info flag);

/**
 * Busca un símbolo en el nivel dado y todos sus niveles padres.
 *
 * @param tabla  Nivel desde el que comienza la búsqueda
 * @param nombre Nombre del símbolo a buscar
 * @param flag   Tipo de símbolo que se busca
 * @return       Puntero a la información del símbolo si se encuentra, NULL si no
 */
Info_Union *buscar_simbolo(Nivel *tabla, char *nombre, Tipo_Info flag);

/**
 * Busca un símbolo solo en el nivel especificado.
 *
 * @param tabla  Nivel donde se realizará la búsqueda
 * @param nombre Nombre del símbolo a buscar
 * @param flag   Tipo de símbolo que se busca
 * @return       Puntero a la información del símbolo si se encuentra, NULL si no
 */
Info_Union *buscar_simbolo_en_nivel(Nivel *tabla, char *nombre, Tipo_Info flag);

/**
 * Busca el último método (DECL_FUNCION) agregado en el nivel dado.
 *
 * @param nivel Nivel donde se buscará
 * @return      Puntero a la información de la última función encontrada, NULL si no hay
 */
Info_Union *buscar_ultimo_metodo(Nivel *nivel);

/**
 * Imprime todos los símbolos de un nivel.
 *
 * @param nivel Nivel cuyos símbolos se imprimirán
 */
void imprimir_simbolos(Nivel *nivel);

#endif