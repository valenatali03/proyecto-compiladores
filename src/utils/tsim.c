#include "../../includes/tsim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../includes/enums.h"

int CANT_NIVELES = 0;

Nivel *crearTabla()
{
    Nivel *tabla = malloc(sizeof(Nivel));
    tabla->head = NULL;
    tabla->parent = NULL;
    return tabla;
}


Nivel *agregarNivel(Nivel *tabla) {
    Nivel *nivel = malloc(sizeof(Nivel));

    nivel->head = NULL;

    nivel->parent = tabla;

    CANT_NIVELES++;

    return nivel;
}

// Desapila el ultimo nivel
Nivel *cerrarNivel(Nivel *tabla)
{
    CANT_NIVELES--;

    return tabla->parent;
}

void agregarSimbolo(Nivel *nivel, Info_Union *info, Tipo_Info flag)
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
            aux= aux->next;
        }
        aux->next = s;
    }
}

Info_Union *buscarSimbolo(Nivel *nivel, char *nombre, Tipo_Info flag)
{
    Nivel *n = nivel;
    while (n != NULL)
    {
        Simbolo *s = n->head;
        while (s != NULL)
        {
            switch (s->flag)
            {
            case ID:
                if (strcmp(s->info->id.nombre, nombre) == 0 && flag == s->flag)
                {
                    return s->info;
                }
                break;

            case FUNCION_DECL:
                if (strcmp(s->info->funcion_decl.nombre, nombre) == 0 && flag == s->flag)
                {
                    return s->info;
                }
                break;

            default:
                break;
            }
            s = s->next;
        }
        n = n->parent;
    }
    return NULL;
}

Info_Union *buscar_en_nivel(Nivel *nivel, char *nombre, Tipo_Info flag)
{
    Simbolo *s = nivel->head;
        while (s != NULL)
        {
            switch (s->flag)
            {
            case ID:
                if (strcmp(s->info->id.nombre, nombre) == 0 && flag == s->flag)
                {
                    return s->info;
                }
                break;

            case FUNCION_DECL:
                if (strcmp(s->info->funcion_decl.nombre, nombre) == 0 && flag == s->flag)
                {
                    return s->info;
                }
                break;

            default:
                break;
            }
            s = s->next;
        }
    return NULL;
}

Info_Union *buscarUltimoMetodo(Nivel* nivel) {
    if (!nivel) return NULL;

    Simbolo* s = nivel->head;
    Info_Union* ultimoMetodo = NULL;

    while (s != NULL) {
        if (s->flag == FUNCION_DECL) {
            ultimoMetodo = s->info;
        }
        s = s->next;
    }

    return ultimoMetodo;
}


void printSimbolos(Nivel *nivel)
{
  Simbolo *aux = nivel->head;
  while (aux != NULL)
  {
    printf("%s ", aux->info->id.nombre);

    aux = aux->next;
  }
}

void printTabla(Nivel *tabla) {
    Nivel *aux = tabla;
    int i = 1;
    while (aux != NULL)
    {
        printf("Nivel %d\n", i);
        printSimbolos(aux);
        aux = aux->parent;
        i++;
    }
}


