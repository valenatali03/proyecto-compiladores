#ifndef PARAMS_H
#define PARAMS_H

typedef struct Info_ID Info_ID;
typedef struct Arbol Arbol;

typedef struct Parametro_Decl
{
    Info_ID *info;
    struct Parametro_Decl *next;
} Parametro_Decl;

typedef struct Parametro_Call
{
    Arbol *expr;
    struct Parametro_Call *next;
} Parametro_Call;

void *agregar_param(void *params, void *data, Tipo_Info tipo_param);

#endif