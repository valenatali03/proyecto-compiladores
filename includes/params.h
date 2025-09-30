#ifndef PARAMS_H
#define PARAMS_H

#include "enums.h"
#include "info.h"

typedef struct Info_Union Info_Union;
typedef struct Arbol Arbol;

typedef struct Parametro_Decl
{
    Info_Union *info;
    struct Parametro_Decl *next;
    int linea;
    int colum;
} Parametro_Decl;

typedef struct Parametro_Call
{
    Arbol *expr;
    struct Parametro_Call *next;
    int linea;
    int colum;
} Parametro_Call;

void *agregar_param(void *params, void *data, Tipo_Info tipo_param, int linea, int colum);

#endif