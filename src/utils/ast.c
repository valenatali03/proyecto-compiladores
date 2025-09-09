#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../includes/ast.h"

Arbol *crear_arbol_operador(char *op, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = OPERADOR;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->info->operador.nombre = strdup(op);
    arbol->info->operador.valor = NULL;
    arbol->info->operador.tipo = tipo;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = NULL;

    return arbol;
}

Arbol *crear_arbol_id(char *id, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = ID;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->info->id.nombre = strdup(id);
    arbol->info->id.valor = NULL;
    arbol->info->id.tipo = VACIO;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = NULL;

    return arbol;
}

Arbol *crear_arbol_literal(void *valor, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = LITERAL;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->info->literal.valor = valor;
    arbol->info->literal.tipo = tipo;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = NULL;

    return arbol;
}

Arbol *crear_arbol_funcion_decl(char *nombre, Tipo tipo, Parametro_Decl *params, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = FUNCION_DECL;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->info->funcion_decl.valor = NULL;
    arbol->info->funcion_decl.nombre = strdup(nombre);
    arbol->info->funcion_decl.tipo = tipo;
    arbol->info->funcion_decl.params = params;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = NULL;

    return arbol;
}

Arbol *crear_arbol_funcion_call(char *nombre, Parametro_Call *params, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = FUNCION_CALL;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->info->funcion_call.nombre = strdup(nombre);
    arbol->info->funcion_call.params = params;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = NULL;

    return arbol;
}

Arbol *crear_arbol_nodo(Tipo_Info tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = tipo;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = NULL;

    return arbol;
}

void inorder(Arbol *arbol)
{
    if (arbol == NULL)
        return;

    inorder(arbol->izq);

    if (arbol->tipo_info == ID)
    {
        printf("ID: %s\n", arbol->info->id.nombre);
    }
    else if (arbol->tipo_info == OPERADOR)
    {
        printf("Operador: %s\n", arbol->info->operador.nombre);
    }
    else if (arbol->tipo_info == LITERAL)
    {
        if (arbol->info->literal.tipo == ENTERO)
        {
            printf("Literal: %d\n", *(int *)arbol->info->literal.valor);
        }
        else if (arbol->info->literal.tipo == BOOL)
        {
            printf("Literal: %s\n", (*(int *)arbol->info->literal.valor) ? "true" : "false");
        }
    }

    inorder(arbol->der);
}

void imprimir_vertical(Arbol *arbol, char *prefijo, int es_ultimo)
{
    if (arbol == NULL)
        return;

    // imprimir prefijo y rama
    printf("%s", prefijo);

    if (es_ultimo)
    {
        printf("└── ");
    }
    else
    {
        printf("├── ");
    }

    // imprimir el nodo según su tipo
    if (arbol->tipo_info == ID)
    {
        printf("ID(%s)\n", arbol->info->id.nombre);
    }
    else if (arbol->tipo_info == OPERADOR)
    {
        printf("Op(%s)\n", arbol->info->operador.nombre);
    }
    else if (arbol->tipo_info == LITERAL)
    {
        if (arbol->info->literal.tipo == ENTERO)
            printf("Lit(%d)\n", *(int *)arbol->info->literal.valor);
        else if (arbol->info->literal.tipo == BOOL)
            printf("Lit(%s)\n", (*(int *)arbol->info->literal.valor) ? "true" : "false");
    }
    else if (arbol->tipo_info == DECLARACION)
    {
        printf("DECLARACION\n");
    }
    else if (arbol->tipo_info == DECLARACIONES)
    {
        printf("DECLARACIONES\n");
    }
    else if (arbol->tipo_info == SENTENCIAS)
    {
        printf("SENTENCIAS\n");
    }
    else if (arbol->tipo_info == FUNCION_DECL)
    {
        printf("FUNCION %s\n", arbol->info->funcion_decl.nombre);
    }
    else if (arbol->tipo_info == RETURN)
    {
        printf("RETURN_INFO\n");
    }
    else if (arbol->tipo_info == ASIGNACION)
    {
        printf("ASIGNACION\n");
    }

    // nuevo prefijo para los hijos
    char nuevo_prefijo[1024];
    strcpy(nuevo_prefijo, prefijo);
    if (es_ultimo)
        strcat(nuevo_prefijo, "    ");
    else
        strcat(nuevo_prefijo, "│   ");

    // contar hijos (izq + der) para saber cuál es el último
    int hijos = 0;
    if (arbol->izq)
        hijos++;
    if (arbol->der)
        hijos++;

    if (arbol->izq)
        imprimir_vertical(arbol->izq, nuevo_prefijo, (hijos == 1 && !arbol->der));
    if (arbol->der)
        imprimir_vertical(arbol->der, nuevo_prefijo, 1);
}