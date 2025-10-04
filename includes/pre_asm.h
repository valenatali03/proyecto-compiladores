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
int CANT_TAG = 0;

extern char **codigo;

static int CANT_LINEAS = 0;

void agregar_instrucciones(Instrucciones *destino, Instrucciones *origen);
void generar_codigo(Arbol *nodo, Instrucciones *instrucciones);
Instrucciones *construir_declaraciones(Arbol *nodo);
Instrucciones *construir_sentencias(Arbol *nodo);
void construir_return(Arbol *nodo, Instrucciones *instrucciones);
void construir_op(Arbol *nodo, Instrucciones *instrucciones);
Tipo_Operador traducir_op(char *op);
void construir_condicional(Arbol *nodo, Instrucciones *instrucciones);
void construir_asignacion(Arbol *nodo, Instrucciones *instrucciones);
void construir_iteracion(Arbol *nodo, Instrucciones *instrucciones);
void construir_funcion_decl(Arbol *nodo, Instrucciones *instrucciones);
void construir_params(Arbol *nodo, Instrucciones *instrucciones);
void construir_funcion_call(Arbol *nodo, Instrucciones *instrucciones);
void insertar_cuadruplo(Cuadruplo *c, Instrucciones *inst);
Simbolo *buscar_resultado(Instrucciones *inst);
int cant_params(Arbol *nodo);
Simbolo *crear_etiqueta(char *nombre);
Simbolo *crear_simbolo(Info_Union *info, Tipo_Info flag);
Instrucciones *crear_lista_instrucciones();
static char *simbolo_a_str(Simbolo *s);
void imprimir_codigo3d(Instrucciones *instrucciones);