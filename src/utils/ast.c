#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../includes/ast.h"

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
    arbol->tipo_info = DECL_FUNCION;
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
    arbol->tipo_info = CALL_FUNCION;
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

void imprimir_nodo_dot(Arbol *arbol, FILE *f)
{
    if (!arbol)
        return;

    char etiqueta[256];
    char color[20];

    // Definir etiqueta y color según el tipo de nodo
    switch (arbol->tipo_info)
    {
    case DECLARACION_VARIABLE:
        snprintf(etiqueta, sizeof(etiqueta), "Decl Variable");
        strcpy(color, "grey");
        break;
    case IF:
        snprintf(etiqueta, sizeof(etiqueta), "IF");
        strcpy(color, "grey");
        break;
    case WHILE:
        snprintf(etiqueta, sizeof(etiqueta), "WHILE");
        strcpy(color, "grey");
        break;
    case DECL_FUNCION:
        snprintf(etiqueta, sizeof(etiqueta), "FuncDecl(%s)", arbol->info->funcion_decl.nombre);
        strcpy(color, "grey");
        break;
    case CALL_FUNCION:
        snprintf(etiqueta, sizeof(etiqueta), "FuncCall(%s)", arbol->info->funcion_call.nombre);
        strcpy(color, "grey");
        break;
    case OPERADOR_BINARIO:
        snprintf(etiqueta, sizeof(etiqueta), "%s", arbol->info->operador.nombre);
        strcpy(color, "white");
        break;
    case OPERADOR_UNARIO:
        snprintf(etiqueta, sizeof(etiqueta), "%s", arbol->info->operador.nombre);
        strcpy(color, "white");
        break;
    default:
        snprintf(etiqueta, sizeof(etiqueta), "%s", tipo_info_str[arbol->tipo_info]);
        strcpy(color, "white");
        break;
    }

    // Imprimir el nodo con color
    fprintf(f, "    n%p [label=\"%s\", style=filled, fillcolor=%s, fontcolor=black];\n",
            (void *)arbol, etiqueta, color);

    // Conectar hijos
    if (arbol->izq)
    {
        fprintf(f, "    n%p -> n%p;\n", (void *)arbol, (void *)arbol->izq);
        imprimir_nodo_dot(arbol->izq, f);
    }
    if (arbol->medio)
    {
        fprintf(f, "    n%p -> n%p;\n", (void *)arbol, (void *)arbol->medio);
        imprimir_nodo_dot(arbol->medio, f);
    }
    if (arbol->der)
    {
        fprintf(f, "    n%p -> n%p;\n", (void *)arbol, (void *)arbol->der);
        imprimir_nodo_dot(arbol->der, f);
    }

    // Parámetros DECL_FUNCION
    if (arbol->tipo_info == DECL_FUNCION && arbol->info->funcion_decl.params)
    {
        Parametro_Decl *p = arbol->info->funcion_decl.params;
        while (p)
        {
            fprintf(f, "    n%p [label=\"Param(%s : %s)\", style=filled, fillcolor=grey];\n",
                    (void *)p,
                    p->info->id.nombre,
                    tipo_str[p->info->id.tipo]);
            fprintf(f, "    n%p -> n%p;\n", (void *)arbol, (void *)p);
            p = p->next;
        }
    }

    // Parámetros CALL_FUNCION
    if (arbol->tipo_info == CALL_FUNCION && arbol->info->funcion_call.params)
    {
        Parametro_Call *p = arbol->info->funcion_call.params;
        while (p)
        {
            if (p->expr)
            {
                fprintf(f, "    n%p -> n%p;\n", (void *)arbol, (void *)p->expr);
                imprimir_nodo_dot(p->expr, f);
            }
            p = p->next;
        }
    }
}

void exportar_ast_a_dot(Arbol *raiz, const char *nombre_archivo)
{
    FILE *f = fopen(nombre_archivo, "w");
    if (!f)
    {
        perror("No se pudo abrir archivo DOT");
        return;
    }
    fprintf(f, "digraph AST {\n");
    fprintf(f, "    node [shape=ellipse, fontname=\"Courier\", fontsize=12];\n");
    imprimir_nodo_dot(raiz, f);
    fprintf(f, "}\n");
    fclose(f);
}
