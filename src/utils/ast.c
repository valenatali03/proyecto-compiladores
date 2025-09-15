#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../includes/ast.h"

static void imprimir_parametros_decl(Parametro_Decl *params, char *prefijo)
{
    int i = 1;
    while (params)
    {
        printf("%s    ├── ParamDecl %d: %s (tipo %s)\n", prefijo, i,
               params->info->nombre, tipo_str[params->info->tipo]);
        params = params->next;
        i++;
    }
}

static void imprimir_parametros_call(Parametro_Call *params, char *prefijo)
{
    int i = 1;
    while (params)
    {
        printf("%s    ├── ParamCall %d:\n", prefijo, i);
        /* imprimir el AST de la expresión del parámetro */
        char nuevo_prefijo[1024];
        strcpy(nuevo_prefijo, prefijo);
        strcat(nuevo_prefijo, "    │   ");
        imprimir_vertical(params->expr, nuevo_prefijo, 1);
        params = params->next;
        i++;
    }
}

Arbol *crear_arbol_operador(char *op, Tipo_Info tipo_op, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = tipo_op;
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

Arbol *crear_arbol_if(Tipo_Info tipo, int linea, int colum, Arbol *izq, Arbol *medio, Arbol *der)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = tipo;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = medio;

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
    else if (arbol->tipo_info == OPERADOR_BINARIO || arbol->tipo_info == OPERADOR_UNARIO)
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

    printf("%s", prefijo);
    if (es_ultimo)
        printf("└── ");
    else
        printf("├── ");

    // Imprimir el nodo según su tipo
    switch (arbol->tipo_info)
    {
    case ID:
        printf("ID(%s)\n", arbol->info->id.nombre);
        break;
    case OPERADOR_BINARIO || OPERADOR_UNARIO:
        printf("Op(%s)\n", arbol->info->operador.nombre);
        break;
    case LITERAL:
        if (arbol->info->literal.tipo == ENTERO)
            printf("Lit(%d)\n", *(int *)arbol->info->literal.valor);
        else if (arbol->info->literal.tipo == BOOL)
            printf("Lit(%s)\n", (*(int *)arbol->info->literal.valor) ? "true" : "false");
        else
            printf("Lit(?)\n");
        break;
    case DECLARACION_VARIABLE:
        printf("DECLARACION\n");
        break;
    case DECLARACIONES_VARIABLES:
        printf("DECLARACIONES\n");
        break;
    case SENTENCIAS:
        printf("SENTENCIAS\n");
        break;
    case FUNCION_DECL:
        printf("FUNCION_DECL %s (tipo %s)\n",
               arbol->info->funcion_decl.nombre,
               tipo_str[arbol->info->funcion_decl.tipo]);
        /* imprimir parámetros */
        imprimir_parametros_decl(arbol->info->funcion_decl.params, prefijo);
        break;
    case FUNCION_CALL:
        printf("FUNCION_CALL %s\n", arbol->info->funcion_call.nombre);
        imprimir_parametros_call(arbol->info->funcion_call.params, prefijo);
        break;
    case RETURN:
        printf("RETURN_INFO\n");
        break;
    case ASIGNACION:
        printf("ASIGNACION\n");
        break;
    default:
        printf("Nodo tipo %s\n", tipo_info_str[arbol->tipo_info]);
        break;
    }

    // construir prefijo para los hijos
    char nuevo_prefijo[1024];
    strcpy(nuevo_prefijo, prefijo);
    if (es_ultimo)
        strcat(nuevo_prefijo, "    ");
    else
        strcat(nuevo_prefijo, "│   ");

    // contar hijos
    int hijos = 0;
    if (arbol->izq)
        hijos++;
    if (arbol->medio)
        hijos++;
    if (arbol->der)
        hijos++;

    // recorrer hijos (izq, medio, der)
    if (arbol->izq)
        imprimir_vertical(arbol->izq, nuevo_prefijo,
                          (hijos == 1 && !arbol->medio && !arbol->der));
    if (arbol->medio)
        imprimir_vertical(arbol->medio, nuevo_prefijo,
                          (hijos == 1 && !arbol->der));
    if (arbol->der)
        imprimir_vertical(arbol->der, nuevo_prefijo, 1);
}
