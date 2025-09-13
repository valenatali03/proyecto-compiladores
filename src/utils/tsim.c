#include "../../includes/tsim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../includes/enums.h"

Nivel *crearTabla()
{
    Nivel *tabla = malloc(sizeof(Nivel));
    tabla->primSim = NULL;
    tabla->sigNivel = NULL;
    tabla->antNivel = NULL;
    CANT_NIVELES++;
    return tabla;
}


Nivel *agregarNivel(Nivel *tabla) {
    Nivel *nivel = malloc(sizeof(Nivel));
    nivel->primSim = NULL;
    nivel->sigNivel = NULL;
    Nivel *aux = tabla;
    while (aux->sigNivel != NULL) {
        aux = aux->sigNivel;
    }
    nivel->antNivel = aux;
    aux->sigNivel = nivel;
    return nivel;
}

// Desapila el ultimo nivel
void cerrarNivel(Nivel *tabla)
{
    Nivel *auxUlt = tabla;
    Nivel *auxAnt;
    while (auxUlt->sigNivel != NULL)
    {
        auxUlt = auxUlt->sigNivel;
    }
    auxAnt = auxUlt->antNivel;
    auxUlt->antNivel = NULL;
    auxAnt->sigNivel = NULL;
    CANT_NIVELES--;
}

void agregarSimbolo(Nivel *nivel, Info_Union *info, Tipo_Info flag)
{
    Simbolo *s = malloc(sizeof(Simbolo));
    s->flag = flag;
    s->info = info;
    s->sig = NULL;
    if (nivel->primSim == NULL)
        nivel->primSim = s;
    else
    {
        Simbolo *elem = nivel->primSim;
        while (elem->sig != NULL)
        {
            elem = elem->sig;
        }
        elem->sig = s;
    }
}

Simbolo *buscarSimbolo(Nivel *nivel, char *nombre, Tipo_Info flag)
{
    Nivel *niv = nivel;
    while (niv != NULL)
    {
        Simbolo *sim = niv->primSim;
        while (sim != NULL)
        {
            switch (sim->flag)
            {
            case ID:
                if (strcmp(sim->info->id.nombre, nombre) == 0)
                {
                    return sim;
                }
                break;

            case FUNCION_DECL:
                if (strcmp(sim->info->funcion_decl.nombre, nombre) == 0)
                {
                    return sim;
                }
                break;

            default:
                break;
            }
            sim = sim->sig;
        }
        niv = niv->antNivel;
    }
    return NULL;
}


void printSimbolos(Nivel *nivel)
{
  Simbolo *aux = nivel->primSim;
  while (aux != NULL)
  {
    printf("%s ", aux->info->id.nombre);

    int tipo = aux->info->id.tipo;
    switch (tipo)
    {
    case 0:
      printf("Entero\n");
      break;
    case 1:
      printf("Bool\n");
      break;
    }

    aux = aux->sig;
  }
}

void printTabla(Nivel *tabla) {
    Nivel *aux = tabla;
    int i = 1;
    while (aux != NULL)
    {
        printf("Nivel %d\n", i);
        printSimbolos(aux);
        aux = aux->sigNivel;
        i++;
    }
}


