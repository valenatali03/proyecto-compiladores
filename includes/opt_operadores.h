#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errores.h"
void optimizar_operaciones(Arbol *arbol);
Arbol *reducir_operador(Arbol *nodo);
void *realizar_operacion_binaria(void *a, void *b, Arbol *nodo_operador);
void *realizar_operacion_unaria(void *a, Arbol *nodo_operador);
void liberar_nodo_arbol(Arbol *nodo);
void optimizar_parametros(Arbol* nodo);




