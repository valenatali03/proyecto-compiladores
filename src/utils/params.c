#include <stdlib.h>
#include "../includes/params.h"

void *agregar_param(void *params, void *data, Tipo_Info tipo_param)
{
    switch (tipo_param)
    {
    case CALL_FUNCION:
        Parametro_Call *lista = (Parametro_Call *)params;
        Arbol *expr = (Arbol *)data;

        if (lista == NULL)
        {
            lista = malloc(sizeof(Parametro_Call));
            lista->expr = expr;
            lista->next = NULL;
            return lista;
        }

        Parametro_Call *aux = lista;
        while (aux->next != NULL)
        {
            aux = aux->next;
        }

        Parametro_Call *nuevo = malloc(sizeof(Parametro_Call));
        nuevo->expr = expr;
        aux->next = nuevo;

        return lista;

        break;

    case DECL_FUNCION:
        Parametro_Decl *parametros = (Parametro_Decl *)params;
        Info_Union *info = (Info_Union *)data;

        if (parametros == NULL)
        {
            parametros = malloc(sizeof(Parametro_Decl));
            parametros->info = info;
            parametros->next = NULL;
            return parametros;
        }

        Parametro_Decl *aux_decl = parametros;

        while (aux_decl->next != NULL)
        {
            aux_decl = aux_decl->next;
        }

        Parametro_Decl *nuevo_decl = malloc(sizeof(Parametro_Decl));
        nuevo_decl->info = info;
        nuevo_decl->next = NULL;
        aux_decl->next = nuevo_decl;

        return parametros;

        break;
    default:
        break;
    }
    return NULL;
}