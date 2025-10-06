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
    struct Instrucciones *next;
} Instrucciones;

extern int CANT_LINEAS;
extern int CANT_TEMP;
extern int CANT_JUMP;
extern int CANT_TAG;

extern char **codigo;
extern Instrucciones *instrucciones;

void agregar_instrucciones(Instrucciones *destino, Instrucciones *origen);
void generar_codigo(Arbol *nodo, Instrucciones *instrucciones);
void construir_declaracion_variables(Arbol *nodo, Instrucciones *instrucciones);
void construir_declaracion_metodos(Arbol *nodo, Instrucciones *instrucciones);
void construir_sentencias(Arbol *nodo, Instrucciones *instrucciones);
void construir_sentencia(Arbol *nodo, Instrucciones *instrucciones);
void construir_return(Arbol *nodo, Instrucciones *instrucciones);
void construir_op(Arbol *nodo, Instrucciones *instrucciones);
Tipo_Operador traducir_op(char *op);
void construir_condicional(Arbol *nodo, Instrucciones *instrucciones);
void construir_asignacion(Arbol *nodo, Instrucciones *instrucciones);
void construir_iteracion(Arbol *nodo, Instrucciones *instrucciones);
void construir_funcion_decl(Arbol *nodo, Instrucciones *instrucciones);
void construir_bloque(Arbol *nodo, Instrucciones *instrucciones);
void construir_params(Arbol *nodo, Instrucciones *instrucciones);
void construir_funcion_call(Arbol *nodo, Instrucciones *instrucciones);
void construir_expresion(Arbol *nodo, Instrucciones *instrucciones);
void insertar_cuadruplo(Cuadruplo *c, Instrucciones *inst);
Simbolo *buscar_resultado(Instrucciones *inst);
int cant_params(Arbol *nodo);
Simbolo *crear_etiqueta(char *nombre);
Simbolo *crear_simbolo(Info_Union *info, Tipo_Info flag);
Instrucciones *crear_lista_instrucciones();
void simbolo_a_str(Simbolo *s, char buffer[64]);
void instrucciones_to_str(Instrucciones *instrucciones);
void imprimir_codigo_ci(FILE *out_ci);