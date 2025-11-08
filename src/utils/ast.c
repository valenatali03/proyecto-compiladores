#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../includes/ast.h"

/**
 * Función auxiliar para la creación de nodos
 * @param tipo_info Tipo de nodo
 * @param linea Linea del código fuente
 * @param colum Columna del código fuente
 * @param izq Hijo izquierdo
 * @param der Hijo derecho
 * @param medio Hijo del medio
 */
static Arbol *crear_nodo_base(Tipo_Info tipo_info, int linea, int colum, Arbol *izq, Arbol *der, Arbol *medio)
{
    Arbol *arbol = malloc(sizeof(Arbol));
    arbol->info = malloc(sizeof(Info_Union));
    arbol->tipo_info = tipo_info;
    arbol->linea = linea;
    arbol->colum = colum;
    arbol->izq = izq;
    arbol->der = der;
    arbol->medio = medio;
    return arbol;
}

Arbol *crear_arbol_operador(char *op, Tipo_Info tipo_op, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(tipo_op, linea, colum, izq, der, NULL);

    arbol->info->operador.nombre = strdup(op);
    arbol->info->operador.valor = NULL;
    arbol->info->operador.tipo = tipo;

    return arbol;
}

Arbol *crear_arbol_id(char *id, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(ID, linea, colum, izq, der, NULL);

    arbol->info->id.nombre = strdup(id);
    arbol->info->id.valor = NULL;
    arbol->info->id.tipo = VACIO;
    arbol->info->id.global = 0;
    arbol->info->id.usos = 0;
    arbol->info->id.offset = -1;
    arbol->info->id.temp = 0;

    return arbol;
}

Arbol *crear_arbol_literal(void *valor, Tipo tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(LITERAL, linea, colum, izq, der, NULL);

    arbol->info->literal.valor = valor;
    arbol->info->literal.tipo = tipo;

    return arbol;
}

Arbol *crear_arbol_funcion_decl(char *nombre, Tipo tipo, Parametro_Decl *params, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(DECL_FUNCION, linea, colum, izq, der, NULL);

    arbol->info->funcion_decl.valor = NULL;
    arbol->info->funcion_decl.nombre = strdup(nombre);
    arbol->info->funcion_decl.tipo = tipo;
    arbol->info->funcion_decl.usos = 0;
    arbol->info->funcion_decl.params = params;
    arbol->info->funcion_decl.cant_params = contar_parametros_decl(params);

    return arbol;
}

Arbol *crear_arbol_funcion_call(char *nombre, Parametro_Call *params, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(CALL_FUNCION, linea, colum, izq, der, NULL);

    arbol->info->funcion_call.nombre = strdup(nombre);
    arbol->info->funcion_call.params = params;
    arbol->info->funcion_call.cant_params = contar_parametros_call(params);

    return arbol;
}

Arbol *crear_arbol_if(Tipo_Info tipo, int linea, int colum, Arbol *izq, Arbol *medio, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(tipo, linea, colum, izq, der, medio);

    return arbol;
}

Arbol *crear_arbol_nodo(Tipo_Info tipo, int linea, int colum, Arbol *izq, Arbol *der)
{
    Arbol *arbol = crear_nodo_base(tipo, linea, colum, izq, der, NULL);

    return arbol;
}

void liberar_arbol(Arbol *nodo) {
    if (!nodo) return;

    liberar_arbol(nodo->izq);
    liberar_arbol(nodo->medio);
    liberar_arbol(nodo->der);

    switch (nodo->tipo_info) {
        case LITERAL: liberar_literal(nodo->info); break;
        case DECLARACION_VARIABLE: liberar_nodo_id(nodo->izq); break;
        case ID: return; break;
        case OPERADOR_BINARIO:
        case OPERADOR_UNARIO: liberar_operador(nodo->info); break;
        case DECL_FUNCION: liberar_funcion_decl(nodo->info); break;
        case CALL_FUNCION: liberar_funcion_call(nodo->info); break;
        default: free(nodo->info); break;
    }

    free(nodo);
}

// Libera un literal
void liberar_literal(Info_Union *info) {
    if (!info) return;
    if (info->literal.valor) free(info->literal.valor);
    free(info);
}

// Libera un nodo ID
void liberar_nodo_id(Arbol *nodo) {
    if (!nodo) return;
    if (nodo->info->id.nombre) free(nodo->info->id.nombre);
    if (nodo->info->id.valor) free(nodo->info->id.valor);
    free(nodo->info);
    free(nodo);
}

// Libera un operador
void liberar_operador(Info_Union *info) {
    if (!info) return;
    if (info->operador.nombre) free(info->operador.nombre);
    if (info->operador.valor) free(info->operador.valor);
    free(info);
}

// Libera parámetros de función
void liberar_parametros_decl(Parametro_Decl *params) {
    while (params) {
        Parametro_Decl *sig = params->next;
        if (params->info->id.nombre) free(params->info->id.nombre);
        if (params->info->id.valor) free(params->info->id.valor);
        free(params->info);
        free(params);
        params = sig;
    }
}

// Libera parámetros de llamada de función
void liberar_parametros_call(Parametro_Call *params) {
    while (params) {
        Parametro_Call *sig = params->next;
        liberar_arbol(params->expr);
        free(params);
        params = sig;
    }
}

// Libera una función declarada
void liberar_funcion_decl(Info_Union *info) {
    if (!info) return;
    if (info->funcion_decl.nombre) free(info->funcion_decl.nombre);
    if (info->funcion_decl.valor) free(info->funcion_decl.valor);
    liberar_parametros_decl(info->funcion_decl.params);
    free(info);
}

// Libera una función llamada
void liberar_funcion_call(Info_Union *info) {
    if (!info) return;
    if (info->funcion_call.nombre) free(info->funcion_call.nombre);
    liberar_parametros_call(info->funcion_call.params);
    free(info);
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
    case LITERAL:
        if (arbol->info->literal.tipo == ENTERO)
            snprintf(etiqueta, sizeof(etiqueta), "%d", *(int *)arbol->info->literal.valor);
        else if (arbol->info->literal.tipo == BOOL)
            snprintf(etiqueta, sizeof(etiqueta), "%s", (*(bool *)arbol->info->literal.valor) ? "true" : "false");
        strcpy(color, "white");
        break;
    case ID:
        snprintf(etiqueta, sizeof(etiqueta), "%s", arbol->info->id.nombre);
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
