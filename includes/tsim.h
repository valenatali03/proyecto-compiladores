#ifndef TSIM_H
#define TSIM_H

#include "ast.h"

extern int CANT_NIVELES;

typedef struct Simbolo
{
    Info_Union *info;
    struct Simbolo *next;
    Tipo_Info flag;
} Simbolo;

typedef struct Nivel
{
    struct Simbolo *head;
    struct Nivel *parent;
} Nivel;

Nivel *crearTabla();

void agregarSimbolo(Nivel *nivel, Info_Union *info, Tipo_Info flag);

Nivel *agregarNivel(Nivel *tabla);

Nivel *cerrarNivel(Nivel *tabla);

Info_Union *buscarSimbolo(Nivel *tabla, char *nombre, Tipo_Info flag);

Info_Union *buscar_en_nivel(Nivel *tabla, char *nombre, Tipo_Info flag);

void printTabla(Nivel *tabla);

Info_Union *buscarUltimoMetodo(Nivel *nivel);

#endif