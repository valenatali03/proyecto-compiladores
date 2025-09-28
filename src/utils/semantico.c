#include "../../includes/semantico.h"
#include <stdio.h>
#include <string.h>
#include "../../includes/errores.h"

extern Nivel *tabla;
int is_main = 0;
int main_has_params = 0;
int main_params_line = 0;
int main_params_colum = 0;

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
        // printf("Variable declarada VACIO.\n");
        return;
    }

    if (buscar_en_nivel(nivelActual, nombre, arbol->izq->tipo_info))
    {
        reportarError(VAR_YA_DECLARADA, arbol->linea, arbol->colum, nombre);
        // printf("Variable %s ya declarada.\n", nombre);
        return;
    }

    Arbol *expr = arbol->der;

    if (procesar_expresion(expr, nivelActual))
    {
        Tipo tExpr = obtener_tipo(expr, nivelActual);
        Tipo tId = arbol->izq->info->id.tipo;
        if (tId != tExpr)
        {
            reportarError(TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, tId, tExpr);
            // printf("Los tipos NO coinciden en la declaracion.\n");
            return;
        }
    }
    else
    {
        printf("Error en la expresion a usar como valor en declaracion\n");
        return;
    }

    agregarSimbolo(nivelActual, arbol->izq->info, arbol->izq->tipo_info);
}

void procesar_declaracion_metodo(Arbol *arbol, Nivel *nivelActual)
{
    if (arbol == NULL)
        return;

    if (arbol->tipo_info == FUNCION_DECL)
    {
        declarar_metodo(arbol, nivelActual);

        Parametro_Decl *params = arbol->info->funcion_decl.params; // Parámetros formales del método

        if (arbol->izq)
        {
            nivelActual = procesar_bloque(arbol->izq, nivelActual, params);
            char *nombre = arbol->info->funcion_decl.nombre;
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
                Nivel *temp = agregarNivel(nivelActual);
                procesar_params(params, temp);
                cerrarNivel(temp);
            }
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

    if (buscar_en_nivel(nivelActual, nombre, arbol->tipo_info))
    {
        reportarError(FUN_YA_DECLARADA, arbol->linea, arbol->colum, nombre);
        // printf("Metodo %s ya declarado.\n", nombre);
    }
    else
    {
        agregarSimbolo(nivelActual, arbol->info, arbol->tipo_info);
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
            return;
        }

        if (buscar_en_nivel(nivelActual, nombre, ID))
        {
            reportarError(VAR_YA_DECLARADA, params->linea, params->colum, nombre); // Arreglar linea y columna
            // printf("Variable %s ya declarada.\n", nombre);
            return;
        }

        agregarSimbolo(nivelActual, params->info, ID);

        params = params->next;
    }
}

Nivel *procesar_bloque(Arbol *arbol, Nivel *nivelActual, Parametro_Decl *params)
{

    Nivel *nuevo = agregarNivel(nivelActual);

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

    return cerrarNivel(nuevo);
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

    if (!procesar_expresion(arbol->medio, nivelActual))
    {
        printf("Error de expresion en condicion de IF\n");
        return 0;
    }

    Tipo t = obtener_tipo(arbol->medio, nivelActual);

    if (t != BOOL)
    {
        reportarError(TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, BOOL, t);
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

int procesar_while(Arbol *arbol, Nivel *nivelActual)
{
    if (!arbol)
        return 0;

    if (!procesar_expresion(arbol->izq, nivelActual))
    {
        printf("Error de expresion en condicion de WHILE\n");
        return 0;
    }

    Tipo t = obtener_tipo(arbol->izq, nivelActual);

    if (t != BOOL)
    {
        reportarError(TIPO_INCOMPATIBLE, arbol->linea, arbol->colum, BOOL, t);
        // printf("Error de tipo en condición de WHILE\n");
        return 0;
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
    case FUNCION_CALL:
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

    Info_Union *simbolo = buscarSimbolo(nivelActual, nombre, arbol->izq->tipo_info);

    if (!simbolo)
    {
        reportarError(VAR_NO_DECLARADA, arbol->linea, arbol->colum, nombre);
        // printf("Asignacion no valida %s no declarada.\n", nombre);
        return 0;
    }

    if (!procesar_expresion(arbol->der, nivelActual))
    {
        printf("Error de tipo en expresion de asignacion.\n");
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

        Info_Union *simbolo = buscarSimbolo(nivelActual, nombre, arbol->tipo_info);

        if (!simbolo)
        {
            reportarError(VAR_NO_DECLARADA, arbol->linea, arbol->colum, nombre);
            // printf("Variable no declarada.\n");
            return 0;
        }
        arbol->info = simbolo;
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
                printf("Número fuera de rango.\n");
                return 0;
            }
        }
        return 1;
        break;
    case FUNCION_CALL:
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
            return t1 == t2;
        }
        else if (strcmp(op, ">") == 0 || strcmp(op, "<") == 0)
        {
            return t1 == ENTERO && t2 == ENTERO;
        }
        else
        {
            return t1 == t2 && t1 == arbol->info->operador.tipo;
        }
        break;

    case OPERADOR_UNARIO:
        if (!procesar_expresion(arbol->izq, nivelActual))
            return 0;

        Tipo t = obtener_tipo(arbol->izq, nivelActual);

        return t == arbol->info->operador.tipo;
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
    case FUNCION_CALL:
        Info_Union *metodo = buscar_en_nivel(tabla, arbol->info->funcion_call.nombre, FUNCION_DECL);
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

    Info_Union *metodo = buscarUltimoMetodo(tabla);

    if (!metodo)
        return;

    Tipo t = metodo->funcion_decl.tipo;

    if (arbol->izq)
    { // return expr ;
        if (t == VACIO)
        {
            printf("Un metodo VACIO no debe retornar expresión.\n");
            return;
        }

        if (!procesar_expresion(arbol->izq, nivelActual))
        {
            printf("Error en la expresion del return\n");
            return;
        }

        Tipo tipoExpr = obtener_tipo(arbol->izq, nivelActual);

        if (t != tipoExpr)
        {
            printf("Error de tipo RETURN\n");
        }
    }
    else
    { // return ;
        if (t != VACIO)
        {
            printf("Error de tipo RETURN\n");
        }
    }
}

int procesar_metodo(Arbol *arbol, Nivel *nivelActual)
{
    Info_Union *metodo = buscar_en_nivel(tabla, arbol->info->funcion_call.nombre, FUNCION_DECL);

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
            printf("Error en la expresion utilizada en la llamada de metodo\n");
            return 0;
        }

        Tipo t2 = obtener_tipo(params_call->expr, nivelActual);

        if (t1 != t2)
        {
            reportarError(TIPO_PARAMETRO, arbol->linea, arbol->colum, arbol->info->funcion_call.nombre, t1, t2);
            // printf("Los tipos de parametros no coinciden.\n");
            return 0;
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
