#include <stddef.h>
#include "helpers.h"
#include "semantico.h"

void recolectarDeclaraciones(Arbol *arbol, Nivel *tabla)
{
    if (!arbol)
        return;

    switch (arbol->tipo_info)
    {
    case DECLARACIONES_VARIABLES:
        procesarDeclaracion(arbol, tabla);
        break;
    case DECLARACIONES_FUNCIONES:
        procesarFuncion(arbol, tabla);
        break;
    default:
        break;
    }

    recolectarDeclaraciones(arbol->izq, tabla);
    recolectarDeclaraciones(arbol->der, tabla);
    cerrarNivel(tabla);
}

void chequearTipos(Arbol *arbol, Nivel *tabla)
{
    if (!arbol)
        return;

    chequearTipos(arbol->izq, tabla);
    chequearTipos(arbol->der, tabla);

    switch (arbol->tipo_info)
    {
    case ASIGNACION:
        procesarAsignacion(arbol, tabla);
        break;
    case OPERADOR:
        procesarOperador(arbol, tabla);
        break;
    case RETURN:
        procesarReturn(arbol, tabla, "");
        break;
    default:
        break;
    }
}

void analisisSemantico(Arbol *arbol, Nivel *tabla)
{
    if (!arbol)
        return;

    procesarFuncion(arbol, tabla);

    recolectarDeclaraciones(arbol->izq, tabla);

    chequearTipos(arbol->der, tabla);
}

int procesarId(Arbol *hijo, Tipo_Info tipoPadre, Simbolo *tabla)
{
    char *nombre = hijo->info->id.nombre;
    Tipo_Info flag = hijo->tipo_info;
    Simbolo *simbolo = buscarSimbolo(tabla, nombre, flag);
    int linea = hijo->linea;
    int colum = hijo->colum;

    switch (tipoPadre)
    {
    case DECLARACION_VARIABLE:

        if (!simbolo)
        {
            agregarSimbolo(tabla, hijo->info, flag);
        }
        else
        {
            reportarError(VAR_YA_DECLARADA, nombre, linea, colum);
        }
        break;

    default:
        if (simbolo)
        {
            hijo->info = simbolo->info;
            return 1;
        }
        else
        {
            reportarError(VAR_NO_DECLARADA, nombre, linea, colum);
            return 0;
        }
        break;
    }

    return 1;
}

void procesarDeclaracion(Arbol *arbol, Simbolo *tabla)
{
    procesarId(arbol->izq, arbol->tipo_info, tabla);
}

void procesarAsignacion(Arbol *arbol, Simbolo *tabla)
{

    if (!procesarId(arbol->izq, arbol->tipo_info, tabla))
    {
        return;
    }

    Tipo tipoIzq = arbol->izq->info->id.tipo;
    Tipo tipoDer;

    int linea = arbol->izq->linea;
    int colum_der = arbol->der->colum;

    switch (arbol->der->tipo_info)
    {
    case ID:
    {
        if (!procesarId(arbol->der, arbol->tipo_info, tabla))
        {
            return;
        }
        tipoDer = arbol->der->info->id.tipo;
        break;
    }
    case LITERAL:
        tipoDer = arbol->der->info->literal.tipo;
        break;
    case OPERADOR:
        tipoDer = arbol->der->info->operador.tipo;
        break;
    
    case FUNCION_CALL:

    break;
    default:
        reportarError(TIPO_INCOMPATIBLE, NULL, linea, colum_der);
        // printf("Error: tipo de nodo derecho no válido.\n");
        return;
    }

    if (tipoIzq != tipoDer)
    {
        reportarError(TIPO_INCOMPATIBLE, NULL, linea, colum_der);
        // printf("Error de tipo tipo izq %d y tipo der %d.\n", tipoIzq, tipoDer);
        return;
    }
}

void procesarOperador(Arbol *arbol, Nivel *tabla)
{
    if (arbol->izq->tipo_info == ID && !procesarId(arbol->izq, arbol->tipo_info, tabla))
    {
        return;
    }
    else if (arbol->der->tipo_info == ID && !procesarId(arbol->der, arbol->tipo_info, tabla))
    {
        return;
    }

    Tipo tipoIzq = obtenerTipoNodo(arbol->izq);
    Tipo tipoDer = obtenerTipoNodo(arbol->der);
    int linea = arbol->linea;
    int colum = arbol->colum;

    if (arbol->izq && arbol->der)
    {
        if (tipoIzq != tipoDer)
        {
            reportarError(TIPO_INCOMPATIBLE, arbol->info->operador.nombre, linea, colum);
            // printf("Error de tipo en operador '%s'.\n", arbol->info->operador.nombre);
            return;
        }
        arbol->info->operador.tipo = tipoIzq;
    }
    else if (arbol->izq)
    {
        arbol->info->operador.tipo = tipoIzq;
    }
    else
    {
        arbol->info->operador.tipo = VACIO;
    }
}

void procesarFuncion(Arbol *arbol, Nivel *tabla)
{
    char *nombre = arbol->info->funcion_decl.nombre;
    Tipo_Info flag = arbol->tipo_info;
    int linea = arbol->linea;
    int colum = arbol->colum;

    if (buscarSimbolo(tabla, nombre, flag) == NULL)
    {
        agregarSimbolo(tabla, arbol->info, flag);
    }
    else
    {
        reportarError(FUN_YA_DECLARADA, nombre, linea, colum);
    }
    recolectarDeclaraciones(arbol, agregarNivel(tabla));
    return;
}

void procesarReturn(Arbol *arbol, Simbolo *tabla, char* nombreFunc)
{

    Simbolo *simbolo_funcion = buscarSimbolo(tabla, nombreFunc, FUNCION_DECL);

    Tipo tipo_funcion = simbolo_funcion->info->funcion_decl.tipo;
    int linea = arbol->linea;
    int colum = arbol->colum;

    if (!arbol->izq && tipo_funcion == VACIO)
    {
        return;
    }
    else if (!arbol->izq && tipo_funcion != VACIO)
    {
        reportarError(TIPO_INCOMPATIBLE, NULL, linea, colum);
        // printf("Error de tipos en return, tipo_funcion no void\n");
        return;
    }

    int colum_izq = arbol->izq->colum;

    Tipo_Info flag = arbol->izq->tipo_info;
    Tipo tipo = obtenerTipoNodo(arbol->izq);

    char *nombre;

    switch (flag)
    {
    case ID:
        procesarId(arbol->izq, arbol->tipo_info, tabla);
        tipo = obtenerTipoNodo(arbol->izq);
        nombre = arbol->info->id.nombre;

        if (tipo_funcion != tipo)
        {
            reportarError(TIPO_INCOMPATIBLE, NULL, linea, colum_izq);
            // printf("Error de tipos en return, variable: %s\n", nombre);
            return;
        }

        break;

    case OPERADOR:
        nombre = arbol->izq->info->operador.nombre;

        if (tipo_funcion != tipo)
        {
            reportarError(TIPO_INCOMPATIBLE, NULL, linea, colum_izq);
            // printf("Error de tipos en return, operacion: %s\n", nombre);
        }

        break;

    case LITERAL:
        if (tipo_funcion != tipo)
        {
            reportarError(TIPO_INCOMPATIBLE, NULL, linea, colum_izq);
            // printf("Error de tipos en return, literal: ");
        }

        break;
    default:
        break;
    }

    arbol->info = simbolo_funcion->info;
}
