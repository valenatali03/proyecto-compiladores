#include "info.h"
#include "tsim.h"
#include "enums.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct Cuadruplo
{
    Tipo_Operador op;
    Simbolo *arg1;
    Simbolo *arg2;
    Simbolo *resultado;
} Cuadruplo;

typedef struct Instrucciones
{
    Cuadruplo *expr;
    Instrucciones *next;
} Instrucciones;

int CANT_TEMP = 0;
int CANT_JUMP = 0;
void generar_codigo(Arbol *arbol, Instrucciones *instrucciones);
void construir_asignacion(Arbol *nodo, Instrucciones *instrucciones);
void construir_return(Arbol *nodo, Instrucciones *instrucciones);
void construir_op(Arbol *nodo, Instrucciones *instrucciones);
void construir_condicional(Arbol *nodo, Instrucciones *instrucciones);
void construir_iteracion(Arbol *nodo, Instrucciones *instrucciones);
void construir_funcion(Arbol *nodo, Instrucciones *instrucciones);