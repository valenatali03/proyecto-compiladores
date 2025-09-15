#ifndef ERRORES_H
#define ERRORES_H

#define MAX_ERRORES 100 // Cantidad máxima de errores que se pueden almacenar
#define MAX_MSG 100 // Tamaño máximo del mensaje de error


/**
 * Códigos de error posibles en el compilador.
 */
typedef enum
{
    VAR_NO_DECLARADA,
    VAR_YA_DECLARADA,
    FUN_NO_DECLARADA,
    FUN_YA_DECLARADA,
    TIPO_INCOMPATIBLE
} CodigoError;


/**
 * Estructura que representa un error detectado.
 */
typedef struct
{
    CodigoError codigo;
    char mensaje[MAX_MSG];
} Error;


/**
 * Reporta un error y lo guarda en la lista de errores.
 *
 * @param codigo Código del error (ver @ref CodigoError).
 * @param info   Información adicional (ej. nombre de variable o función).
 * @param linea  Número de línea donde ocurrió el error.
 * @param colum  Número de columna donde ocurrió el error.
 */
void reportarError(CodigoError codigo, const char *info, int linea, int colum);


/**
 * Muestra en consola todos los errores reportados.
 */
void mostrarErrores(void);

#endif