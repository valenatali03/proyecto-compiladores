#include "../../includes/semantico.h"
#include <stdio.h>
#include <string.h>
#include "../../includes/errores.h"

Nivel *tabla = NULL;
int is_main = 0;
int main_has_params = 0;
int main_params_line = 0;
int main_params_colum = 0;
Simbolo *ult_metodo = NULL;

extern int CANT_NIVELES;

void analisis_semantico(Arbol *arbol, Nivel *nivelActual)
{
    if (arbol == NULL)
        return;

    if (arbol->izq)
    {
        procesar_declaracion_variable(arbol->izq, nivelActual);
    }

    if (arbol->der)
    {
        procesar_declaracion_metodo(arbol->der, nivelActual);
    }

    if (!is_main)
    {
        reportarError(MAIN_NO_DECLARADO, arbol->linea, arbol->colum);
        // printf("Error: No se definio funcion main\n");
    }

    if (main_has_params)
    {
        reportarError(MAIN_CON_PARAMS, main_params_line, main_params_colum);
        // printf("Error: Funcion main no puede tener parametros \n");
    }

    nivelActual = cerrar_nivel(nivelActual);
}

void procesar_declaracion_variable(Arbol *arbol, Nivel *nivelActual)
{
    if (arbol == NULL)
        return;

    if (arbol->izq)
    {
        procesar_declaracion_variable(arbol->izq, nivelActual);
    }

    if (arbol->der)
    {
        procesar_declaracion_variable(arbol->der, nivelActual);
    }

    if (arbol->tipo_info == DECLARACION_VARIABLE)
    {
        declarar_variable(arbol, nivelActual);
    }
}

void declarar_variable(Arbol *arbol, Nivel *nivelActual)
{

    char *nombre = arbol->izq->info->id.nombre;

    if (arbol->izq->info->id.tipo == VACIO)
    {
        reportarError(VAR_VACIO, arbol->linea, arbol->colum, nombre);
    }

    if (buscar_simbolo_en_nivel(nivelActual, nombre, arbol->izq->tipo_info))
    {
        reportarError(VAR_YA_DECLARADA, arbol->linea, arbol->colum, nombre);
        return;
    }

    Arbol *expr = arbol->der;

    if (CANT_NIVELES == 0) // Variable global
    {
        arbol->izq->info->id.global = 1;
        if (procesar_declaracion_variable_global(expr))
        {
            reportarError(VAR_GLOBAL_NO_CONST, arbol->linea, arbol->colum, nombre);
            return;
        }
    }

    if (procesar_expresion(expr, nivelActual))
    {
        Tipo tExpr = obtener_tipo(expr, nivelActual);
        Tipo tId = arbol->izq->info->id.tipo;
        if (tId != tExpr)
        {
            reportarError(TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, tId, tExpr);
        }
    }

    agregar_simbolo(nivelActual, arbol->izq->info, arbol->izq->tipo_info);
}

int procesar_declaracion_variable_global(Arbol *arbol)
{
    if (arbol == NULL)
    {
        return 0;
    }

    Tipo_Info t_info = arbol->tipo_info;

    if (t_info == CALL_FUNCION || t_info == ID)
    {
        return 1;
    }

    if (procesar_declaracion_variable_global(arbol->izq))
    {
        return 1;
    }

    if (procesar_declaracion_variable_global(arbol->der))
    {
        return 1;
    }

    return 0;
}

void procesar_declaracion_metodo(Arbol *arbol, Nivel *nivelActual)
{
    if (arbol == NULL)
        return;

    if (arbol->tipo_info == DECL_FUNCION)
    {
        declarar_metodo(arbol, nivelActual);

        Parametro_Decl *params = arbol->info->funcion_decl.params; // Parámetros formales del método

        if (arbol->izq)
        {
            nivelActual = procesar_bloque(arbol->izq, nivelActual, params);
            char *nombre = arbol->info->funcion_decl.nombre;
            arbol->info->funcion_decl.esExterna = 0;
            if (buscar_return(arbol->izq) == 0 && arbol->info->funcion_decl.tipo != VACIO)
            {
                reportarError(FUN_SIN_RETURN, arbol->linea, arbol->colum, nombre);
                // printf("Error: Un método que debería retornar un valor no lo hace.\n");
                return;
            }
        }
        else
        { // Método extern
            if (params)
            {
                Nivel *temp = abrir_nivel(nivelActual);
                procesar_params(params, temp);
                cerrar_nivel(temp);
            }
            arbol->info->funcion_decl.esExterna = 1;
        }

        return;
    }

    if (arbol->izq)
    {
        procesar_declaracion_metodo(arbol->izq, nivelActual);
    }

    if (arbol->der)
    {
        procesar_declaracion_metodo(arbol->der, nivelActual);
    }
}

void declarar_metodo(Arbol *arbol, Nivel *nivelActual)
{

    char *nombre = arbol->info->funcion_decl.nombre;

    procesar_main(nombre, arbol->info->funcion_decl.params);

    if (strcmp(nombre, "main") == 0)
    {
        arbol->info->funcion_decl.usos++;
    }

    if (buscar_simbolo_en_nivel(nivelActual, nombre, arbol->tipo_info))
    {
        reportarError(FUN_YA_DECLARADA, arbol->linea, arbol->colum, nombre);
        // printf("Metodo %s ya declarado.\n", nombre);
    }
    else
    {
        agregar_simbolo(nivelActual, arbol->info, arbol->tipo_info);
        ult_metodo = arbol->info;
    }
}

void procesar_params(Parametro_Decl *params, Nivel *nivelActual)
{
    while (params)
    {
        char *nombre = params->info->id.nombre;

        Tipo t = params->info->id.tipo;

        if (t == VACIO)
        {
            reportarError(VAR_VACIO, params->linea, params->colum, nombre);
        }

        if (buscar_simbolo_en_nivel(nivelActual, nombre, ID))
        {
            reportarError(VAR_YA_DECLARADA, params->linea, params->colum, nombre); // Arreglar linea y columna
            // printf("Variable %s ya declarada.\n", nombre);
        }

        agregar_simbolo(nivelActual, params->info, ID);

        params->info->id.usos++;
        params = params->next;
    }
}

Nivel *procesar_bloque(Arbol *arbol, Nivel *nivelActual, Parametro_Decl *params)
{

    Nivel *nuevo = abrir_nivel(nivelActual);

    if (params)
    {
        procesar_params(params, nuevo);
    }

    if (arbol)
    {
        if (arbol->izq)
        {
            procesar_declaracion_variable(arbol->izq, nuevo);
        }

        if (arbol->der)
        {
            procesar_statements(arbol->der, nuevo);
        }
    }

    return cerrar_nivel(nuevo);
}

void procesar_statements(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return;

    if (arbol->izq)
    {
        procesar_statement(arbol->izq, nivelActual);
    }

    if (arbol->der)
    {
        procesar_statement(arbol->der, nivelActual);
    }
}

int procesar_if(Arbol *arbol, Nivel *nivelActual)
{

    if (!arbol)
        return 0;

    if (procesar_expresion(arbol->medio, nivelActual))
    {
        Tipo t = obtener_tipo(arbol->medio, nivelActual);

        if (t != BOOL)
        {
            reportarError(TIPO_INCOMPATIBLE, arbol->medio->linea, arbol->medio->colum, BOOL, t);
            // printf("Error de tipo en condición de IF\n");
            return 0;
        }

        procesar_bloque(arbol->izq, nivelActual, NULL);

        if (arbol->der)
        {
            procesar_bloque(arbol->der, nivelActual, NULL);
        }

        return 1;
    }

    return 1;
}

int procesar_while(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return 0;

    if (procesar_expresion(arbol->izq, nivelActual))
    {
        Tipo t = obtener_tipo(arbol->izq, nivelActual);

        if (t != BOOL)
        {
            reportarError(TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, BOOL, t);
            // printf("Error de tipo en condición de WHILE\n");
            return 0;
        }
    }

    procesar_bloque(arbol->der, nivelActual, NULL);

    return 1;
}

void procesar_statement(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return;

    switch (arbol->tipo_info)
    {
    case IF:
        procesar_if(arbol, nivelActual);
        break;
    case WHILE:
        procesar_while(arbol, nivelActual);
        break;
    case BLOQUE:
        procesar_bloque(arbol, nivelActual, NULL);
        break;
    case ASIGNACION:
        procesar_asignacion(arbol, nivelActual);
        break;
    case SENTENCIAS:
        procesar_statements(arbol, nivelActual);
        break;
    case RETURN:
        procesar_return(arbol, nivelActual);
        break;
    case CALL_FUNCION:
        procesar_metodo(arbol, nivelActual);
        break;
    default:
        break;
    }
}

int procesar_asignacion(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return 0;

    char *nombre = arbol->izq->info->id.nombre;

    Info_Union *simbolo = buscar_simbolo(nivelActual, nombre, arbol->izq->tipo_info);

    if (!simbolo)
    {
        reportarError(VAR_NO_DECLARADA, arbol->linea, arbol->colum, nombre);
        // printf("Asignacion no valida %s no declarada.\n", nombre);
        return 0;
    }

    if (!procesar_expresion(arbol->der, nivelActual))
    {
        return 0;
    }

    Tipo tExpr = obtener_tipo(arbol->der, nivelActual);

    Tipo tId = simbolo->id.tipo;

    if (tId != tExpr)
    {
        reportarError(TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, tId, tExpr);
        // printf("Error de tipo en asignacion: %s es de tipo %d pero se asigna tipo %d.\n", nombre, tId, tExpr);
        return 0;
    }

    arbol->izq->info = simbolo; // Vincular nodo del arbol
    simbolo->id.usos++;
    return 1;
}

int procesar_expresion(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return 0;

    switch (arbol->tipo_info)
    {
    case ID:
        char *nombre = arbol->info->id.nombre;

        Info_Union *simbolo = buscar_simbolo(nivelActual, nombre, arbol->tipo_info);

        if (!simbolo)
        {
            reportarError(VAR_NO_DECLARADA, arbol->linea, arbol->colum, nombre);
            // printf("Variable no declarada.\n");
            return 0;
        }
        arbol->info = simbolo;
        arbol->info->id.usos++;
        return 1;
        break;
    case OPERADOR_BINARIO:
        return procesar_operador(arbol, nivelActual);
        break;
    case OPERADOR_UNARIO:
        return procesar_operador(arbol, nivelActual);
        break;
    case LITERAL:
        if (arbol->info->literal.tipo == ENTERO)
        {
            int valor = *(int *)arbol->info->literal.valor;
            if (valor > 2147483647 || valor < -2147483647)
            {
                reportarError(NUM_FUERA_RANGO, arbol->linea, arbol->colum);
                return 0;
            }
        }
        return 1;
        break;
    case CALL_FUNCION:
        return procesar_metodo(arbol, nivelActual);
        break;
    default:
        return 0;
        break;
    }
}

int procesar_operador(Arbol *arbol, Nivel *nivelActual)
{

    if (!arbol)
        return 0;

    switch (arbol->tipo_info)
    {
    case OPERADOR_BINARIO:

        if (!procesar_expresion(arbol->izq, nivelActual) || !procesar_expresion(arbol->der, nivelActual))
            return 0;

        Tipo t1 = obtener_tipo(arbol->izq, nivelActual);
        Tipo t2 = obtener_tipo(arbol->der, nivelActual);

        char *op = arbol->info->operador.nombre;

        if (strcmp(op, "==") == 0)
        {
            if (!(t1 == t2))
            {
                reportarError(OP_BINARIO, arbol->linea, arbol->colum, op, t1, t2);
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else if (strcmp(op, ">") == 0 || strcmp(op, "<") == 0)
        {
            if (!(t1 == ENTERO && t2 == ENTERO))
            {
                reportarError(OP_BINARIO, arbol->linea, arbol->colum, op, t1, t2);
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            if (!(t1 == t2 && t1 == arbol->info->operador.tipo))
            {
                reportarError(OP_BINARIO, arbol->linea, arbol->colum, op, t1, t2);
                return 0;
            }
            else
            {
                return 1;
            }
        }
        break;

    case OPERADOR_UNARIO:
        if (!procesar_expresion(arbol->izq, nivelActual))
            return 0;

        Tipo t = obtener_tipo(arbol->izq, nivelActual);

        if (!(t == arbol->info->operador.tipo))
        {
            reportarError(OP_UNARIO, arbol->linea, arbol->colum, arbol->info->operador.nombre, t);
            return 0;
        }
        else
        {
            return 1;
        }
        break;
    default:
        return 0;
        break;
    }
}

Tipo obtener_tipo(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return VACIO;

    switch (arbol->tipo_info)
    {
    case ID:
        return arbol->info->id.tipo;
        break;
    case OPERADOR_BINARIO:
        return arbol->info->operador.tipo;
        break;
    case OPERADOR_UNARIO:
        return arbol->info->operador.tipo;
        break;
    case LITERAL:
        return arbol->info->literal.tipo;
        break;
    case CALL_FUNCION:
        Info_Union *metodo = buscar_simbolo_en_nivel(tabla, arbol->info->funcion_call.nombre, DECL_FUNCION);
        return metodo->funcion_decl.tipo;
        break;
    default:
        return VACIO;
        break;
    }
}

void procesar_return(Arbol *arbol, Nivel *nivelActual)
{

    if (!arbol)
        return;

    Info_Union *metodo = ult_metodo;

    if (!metodo)
        return;

    Tipo t = metodo->funcion_decl.tipo;

    if (arbol->izq)
    { // return expr ;
        if (!procesar_expresion(arbol->izq, nivelActual))
        {
            return;
        }

        Tipo tipoExpr = obtener_tipo(arbol->izq, nivelActual);

        if (t != tipoExpr)
        {
            reportarError(RETURN_TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, metodo->funcion_decl.nombre, t, tipoExpr);
            return;
        }
    }
    else
    { // return ;
        if (t != VACIO)
        {
            reportarError(RETURN_TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, metodo->funcion_decl.nombre, t, VACIO);
            return;
        }
    }
}

int procesar_metodo(Arbol *arbol, Nivel *nivelActual)
{
    Info_Union *metodo = buscar_simbolo_en_nivel(tabla, arbol->info->funcion_call.nombre, DECL_FUNCION);

    if (!metodo)
    {
        reportarError(FUN_NO_DECLARADA, arbol->linea, arbol->colum, arbol->info->funcion_call.nombre);
        // printf("Llamada de método no declarado.\n");
        return 0;
    }

    Parametro_Decl *params_decl = metodo->funcion_decl.params;
    Parametro_Call *params_call = arbol->info->funcion_call.params;

    if (!params_decl && !params_call)
    {
        metodo->funcion_decl.usos++;
        return 1;
    }

    if ((!params_decl && params_call) || (params_decl && !params_call))
    {
        reportarError(CANT_PARAMETROS, arbol->linea, arbol->colum, arbol->info->funcion_call.nombre);
        // printf("Error: cantidad de parámetros incorrecta en llamada.\n");
        return 0;
    }

    while (params_decl && params_call)
    {
        Tipo t1 = params_decl->info->id.tipo;

        if (!procesar_expresion(params_call->expr, nivelActual))
        {
            return 0;
        }

        Tipo t2 = obtener_tipo(params_call->expr, nivelActual);

        if (t1 != t2)
        {
            reportarError(TIPO_PARAMETRO, arbol->linea, arbol->colum, arbol->info->funcion_call.nombre, t1, t2);
            // printf("Los tipos de parametros no coinciden.\n");
        }

        params_decl = params_decl->next;
        params_call = params_call->next;
    }

    // Si uno de los dos no terminó al mismo tiempo, error
    if (params_decl || params_call)
    {
        reportarError(CANT_PARAMETROS, arbol->linea, arbol->colum, arbol->info->funcion_call.nombre);
        // printf("Cantidad de parámetros no coincide\n");
        return 0;
    }
    arbol->info->funcion_call.cantVariables = metodo->funcion_decl.cantVariables;
    arbol->info->funcion_call.esExterna = metodo->funcion_decl.esExterna;
    metodo->funcion_decl.usos++;
    return 1;
}

int buscar_return(Arbol *sentencias)
{
    if (sentencias == NULL)
    {
        return 0;
    }
    else if (sentencias->tipo_info == RETURN)
    {
        return 1;
    }
    else if (sentencias->tipo_info == IF || (sentencias->izq && sentencias->izq->tipo_info == WHILE) || (sentencias->der && sentencias->der->tipo_info == WHILE))
    {
        return (buscar_return(sentencias->izq) && buscar_return(sentencias->der));
    }
    else
    {
        return buscar_return(sentencias->izq) || buscar_return(sentencias->der);
    }
}

void procesar_main(char *nombre, Parametro_Decl *params)
{
    if (strcmp(nombre, "main") == 0 && params)
    {
        is_main = 1;
        main_has_params = 1;
        main_params_line = params->linea;
        main_params_colum = params->colum;
    }

    if (strcmp(nombre, "main") == 0)
    {
        is_main = 1;
    }
}
