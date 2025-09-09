#include <stdlib.h>
#include "../includes/params.h"

void *agregar_param(void *params, void *data, Tipo_Info tipo_param)
{
    switch (tipo_param)
    {
    case FUNCION_CALL:
        Parametro_Call *lista = (Parametro_Call *)params;
        Arbol *expr = (Arbol *)data;
        Parametro_Call *aux = lista;
        while (aux->next != NULL)
        {
            aux = aux->next;
        }

        Parametro_Call *nuevo = malloc(sizeof(Parametro_Call));
        nuevo->expr = expr;
        aux->next = nuevo;

        return aux;

        break;

    case FUNCION_DECL:
        Parametro_Decl *parametros = (Parametro_Decl *)params;
        Info_ID *info = (Info_ID *)data;
        Parametro_Decl *aux_decl = parametros;

        while (aux_decl->next != NULL)
        {
            aux_decl = aux_decl->next;
        }

        Parametro_Decl *nuevo_decl = malloc(sizeof(Parametro_Decl));
        nuevo_decl->info = info;
        aux_decl->next = nuevo_decl;

        return aux_decl;

        break;
    default:
        break;
    }
}