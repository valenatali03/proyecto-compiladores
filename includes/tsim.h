#include "ast.h"

int CANT_NIVELES = 0;

typedef struct Simbolo
{
    Info_Union* info;
    struct Simbolo* sig;
    Tipo_Info flag;
} Simbolo;

typedef struct Nivel {
    struct Simbolo* primSim;
    struct Nivel* sigNivel;
    struct Nivel* antNivel;
} Nivel;

Nivel* crearTabla();

void agregarSimbolo(Nivel* nivel, Info_Union* info, Tipo_Info flag);

Nivel *agregarNivel(Nivel* tabla);

void cerrarNivel(Nivel* tabla);

Simbolo* buscarSimbolo(Nivel* tabla, char* nombre, Tipo_Info flag);

void printTabla(Nivel* tabla);