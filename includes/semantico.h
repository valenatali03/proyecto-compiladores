#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"
#include "tsim.h"

void analisis_semantico(Arbol *arbol, Nivel *tabla);

void procesar_declaracion_variable(Arbol *arbol, Nivel *tabla);

void declarar_variable(Arbol *arbol, Nivel *tabla);

void procesar_declaracion_metodo(Arbol *arbol, Nivel *tabla);

void declarar_metodo(Arbol *arbol, Nivel *tabla);

void procesar_params(Parametro_Decl *params, Nivel *nivelActual);

Nivel *procesar_bloque(Arbol *arbol, Nivel *nivelActual, Parametro_Decl *params);

void procesar_statements(Arbol *arbol, Nivel *tabla);

int procesar_if(Arbol *arbol, Nivel *nivelActual);

int procesar_while(Arbol *arbol, Nivel *nivelActual);

void procesar_statement(Arbol *arbol, Nivel *tabla);

int procesar_asignacion(Arbol *arbol, Nivel *tabla);

int procesar_expresion(Arbol *arbol, Nivel *tabla);

int procesar_operador(Arbol *arbol, Nivel *tabla);

Tipo obtener_tipo(Arbol *arbol, Nivel *tabla);

void procesar_return(Arbol *arbol, Nivel *nivelActual);

int procesar_metodo(Arbol *arbol, Nivel *nivelActual);

int buscar_return(Arbol *sentencias);

void procesar_main(char *nombre, Parametro_Decl *params);

#endif