#include "../includes/tsim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/enums.h"

int CANT_NIVELES = 0;

Nivel *crear_tabla()
{
    Nivel *tabla = malloc(sizeof(Nivel));
    tabla->head = NULL;
    tabla->parent = NULL;
    return tabla;
}

Nivel *abrir_nivel(Nivel *tabla)
{
    Nivel *nivel = malloc(sizeof(Nivel));

    nivel->head = NULL;

    nivel->parent = tabla;

    CANT_NIVELES++;

    return nivel;
}

void liberar_nivel(Nivel *tabla)
{
    Simbolo *s = tabla->head;

    while (s != NULL)
    {
        Simbolo *aux = s;
        s = s->next;
        free(aux);
    }

    free(tabla);
}

Nivel *cerrar_nivel(Nivel *tabla)
{
    CANT_NIVELES--;

    Nivel* nivel_superior = tabla->parent;

    liberar_nivel(tabla);

    return nivel_superior;
}

void agregar_simbolo(Nivel *nivel, Info_Union *info, Tipo_Info flag)
{

    Simbolo *s = malloc(sizeof(Simbolo));
    s->flag = flag;
    s->info = info;
    s->next = NULL;

    if (nivel->head == NULL)
        nivel->head = s;
    else
    {
        Simbolo *aux = nivel->head;
        while (aux->next != NULL)
        {
            aux = aux->next;
        }
        aux->next = s;
    }
}

Info_Union *buscar_simbolo(Nivel *nivel, char *nombre, Tipo_Info flag)
{
    for (Nivel *n = nivel; n != NULL; n = n->parent)
    {
        for (Simbolo *s = n->head; s != NULL; s = s->next)
        {
            switch (s->flag)
            {
            case ID:
                if (flag == ID && strcmp(s->info->id.nombre, nombre) == 0)
                {
                    return s->info;
                }
                break;

            case DECL_FUNCION:
                if (flag == DECL_FUNCION && strcmp(s->info->funcion_decl.nombre, nombre) == 0)
                {
                    return s->info;
                }
                break;

            default:
                break;
            }
        }
    }
    return NULL;
}

Info_Union *buscar_simbolo_en_nivel(Nivel *nivel, char *nombre, Tipo_Info flag)
{

    for (Simbolo *s = nivel->head; s != NULL; s = s->next)
    {
        switch (s->flag)
        {
        case ID:
            if (flag == ID && strcmp(s->info->id.nombre, nombre) == 0)
            {
                return s->info;
            }
            break;

        case DECL_FUNCION:
            if (flag == DECL_FUNCION && strcmp(s->info->funcion_decl.nombre, nombre) == 0)
            {
                return s->info;
            }
            break;

        default:
            break;
        }
    }
    return NULL;
}

Info_Union *buscar_ultimo_metodo(Nivel *nivel)
{
    if (!nivel)
        return NULL;

    Simbolo *s = nivel->head;
    Info_Union *ultimoMetodo = NULL;

    while (s != NULL)
    {
        if (s->flag == DECL_FUNCION)
        {
            ultimoMetodo = s->info;
        }
        s = s->next;
    }

    return ultimoMetodo;
}

void imprimir_simbolos(Nivel *nivel)
{
    for (Simbolo *aux = nivel->head; aux != NULL; aux = aux->next)
    {
        switch (aux->flag)
        {
        case ID:
            printf("ID : %s ", aux->info->id.nombre);
            break;

        case DECL_FUNCION:
            printf("FUNCIÓN : %s() ", aux->info->funcion_decl.nombre);
            break;

        default:
            break;
        }
    }
    printf("\n");
}
