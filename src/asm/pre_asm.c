#include "../../includes/pre_asm.h"
#include "string.h"

void agregar_instrucciones(Instrucciones *destino, Instrucciones *origen) {
    if (!origen) return;
    Instrucciones *aux = destino;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    aux->next = origen;
}

void generar_codigo(Arbol *arbol, Instrucciones *instrucciones)
{
    if (!arbol) return;

    Instrucciones *decl = NULL;
    Instrucciones *stmt = NULL;

    if (arbol->izq) {
        decl = construir_declaraciones(arbol->izq);
        agregar_instrucciones(instrucciones, decl);
    }
    if (arbol->der) {
        stmt = construir_sentencias(arbol->der);
        agregar_instrucciones(instrucciones, stmt);
    }
}


Instrucciones *construir_declaraciones(Arbol *arbol)
{
    if (arbol == NULL)
        return NULL;

    Instrucciones *decl = NULL;
    switch (arbol->tipo_info)
    {
    case DECL_FUNCION:
        construir_funcion_decl(arbol, decl);
        break;

    case DECLARACION_VARIABLE:
        construir_asignacion(arbol, decl);
        break;

    default:
        Instrucciones *izq = construir_declaraciones(arbol->izq);
        Instrucciones *der = construir_declaraciones(arbol->der);
        Instrucciones *aux = izq;
        while (aux->next != NULL)
        {
            aux = aux->next;
        }
        aux->next = der;
        break;
    }
    return decl;
}

Instrucciones *construir_sentencias(Arbol *arbol)
{
    if (arbol == NULL)
        return NULL;

    Instrucciones *stmt = NULL;
    switch (arbol->tipo_info)
    {
    case CALL_FUNCION:
        construir_funcion_call(arbol, stmt);
        break;
    case ASIGNACION:
        construir_asignacion(arbol, stmt);
        break;
    case IF:
        construir_condicional(arbol, stmt);
        break;
    case WHILE:
        construir_iteracion(arbol, stmt);
        break;
    case RETURN:
        construir_return(arbol, stmt);
        break;
    default:
        Instrucciones *izq = construir_sentencias(arbol->izq);
        Instrucciones *der = construir_sentencias(arbol->der);
        Instrucciones *aux = izq;
        while (aux->next != NULL)
        {
            aux = aux->next;
        }
        aux->next = der;
        break;
    }
    return stmt;
}

void construir_return(Arbol *nodo, Instrucciones *instrucciones)
{
    Cuadruplo *ret = malloc(sizeof(Cuadruplo));
    ret->op = RET;

    switch (nodo->izq->tipo_info)
    {
    case ID:
    case LITERAL:
        ret->arg1 = nodo->izq->info;
        break;
    case OPERADOR_BINARIO:
    case OPERADOR_UNARIO:
        construir_op(nodo->izq, instrucciones);
        ret->arg1 = buscar_resultado(instrucciones);
        break;
    case CALL_FUNCION:
        construir_funcion_call(nodo->izq, instrucciones);
        ret->arg1 = buscar_resultado(instrucciones);
        break;
    default:
        break;
    }

    insertar_cuadruplo(ret, instrucciones);
}

void construir_op(Arbol *nodo, Instrucciones *instrucciones)
{
    if (nodo->izq)
    {
        construir_op(nodo->izq, instrucciones);
    }
    if (nodo->der)
    {
        construir_op(nodo->der, instrucciones);
    }
    Cuadruplo *cuad = malloc(sizeof(Cuadruplo));
    if (nodo->tipo_info == OPERADOR_BINARIO)
    {
        cuad->op = traducir_op(nodo->info->operador.nombre);
        switch (nodo->izq->tipo_info)
        {
        case OPERADOR_BINARIO:
            cuad->arg1 = buscar_resultado(instrucciones); // TODO
            break;

        default:
            cuad->arg1 = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
            break;
        }
        switch (nodo->der->tipo_info)
        {
        case OPERADOR_BINARIO:
            cuad->arg2 = buscar_resultado(instrucciones);
            break;
        default:
            cuad->arg2 = crear_simbolo(nodo->der->info, nodo->der->tipo_info);
            break;
        }
    }
    else if (nodo->tipo_info == OPERADOR_UNARIO)
    {
        cuad->op = traducir_op(nodo->info->operador.nombre);
        switch (nodo->izq->tipo_info)
        {
        case OPERADOR_BINARIO:
            cuad->arg1 = buscar_resultado(instrucciones); // TODO
            break;

        default:
            cuad->arg1 = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
            break;
        }
    }
    insertar_cuadruplo(cuad, instrucciones); // TODO
}

Tipo_Operador traducir_op(char *op)
{
    if (strcmp(op, "+") == 0)
        return ADD;
    if (strcmp(op, "-") == 0)
        return MINUS;
    if (strcmp(op, "/") == 0)
        return DIV;
    if (strcmp(op, "*") == 0)
        return MULT;
    if (strcmp(op, '%') == 0)
        return MOD;
    if (strcmp(op, "==") == 0)
        return COMP;
    if (strcmp(op, "&&") == 0)
        return AND;
    if (strcmp(op, "||") == 0)
        return OR;
    if (strcmp(op, "<") == 0)
        return LT;
    if (strcmp(op, ">") == 0)
        return GT;
    if (strcmp(op, "=") == 0)
        return MOV;
    if (strcmp(op, "!") == 0)
        return NOT;
    return NULL;
}

void construir_condicional(Arbol *nodo, Instrucciones *instrucciones)
{
    if (nodo->medio)
    {
        construir_op(nodo->medio, instrucciones);
    }

    Cuadruplo *cuad = malloc(sizeof(Cuadruplo));
    cuad->op = IF_FALSE;
    cuad->arg1 = buscar_resultado(instrucciones);
    cuad->arg2 = NULL;
    // Etiqueta para saltear el bloque verdadero
    cuad->resultado = crear_simbolo(NULL, ETIQUETA);
    insertar_cuadruplo(cuad, instrucciones);
    generar_codigo(nodo->izq, instrucciones);
    if (nodo->der)
    {
        // Construyo el cuadruplo para saltar al final si pasé por el bloque verdadero.
        Cuadruplo *end_if = malloc(sizeof(Cuadruplo));
        end_if->op = JMP;
        end_if->resultado = crear_simbolo(NULL, ETIQUETA);
        insertar_cuadruplo(end_if, instrucciones);
        // Construyo el cuadruplo que contiene la etiqueta del else.
        Cuadruplo *els = malloc(sizeof(Cuadruplo));
        els->op = TAG;
        els->resultado = cuad->resultado;
        insertar_cuadruplo(els, instrucciones);
        // Genero el codigo del else
        generar_codigo(nodo->der, instrucciones);
        // Construyo el cuadruplo que contiene el fin del else
        Cuadruplo *end_else = malloc(sizeof(Cuadruplo));
        end_else->op = TAG;
        end_else->resultado = end_if->resultado;
        insertar_cuadruplo(end_else, instrucciones);
    }
    else
    {
        // if sin else
        Cuadruplo *end_if = malloc(sizeof(Cuadruplo));
        end_if->op = TAG;
        end_if->resultado = cuad->resultado;
        insertar_cuadruplo(end_if, instrucciones);
    }
}

void construir_asignacion(Arbol *nodo, Instrucciones *instrucciones)
{
    Cuadruplo *mov = malloc(sizeof(Cuadruplo));
    mov->op = MOV;
    mov->resultado = nodo->izq->info;
    if (nodo->der->tipo_info == LITERAL || nodo->der->tipo_info == ID)
    {
        mov->arg1 = nodo->der->info;
    }
    else if (nodo->der->tipo_info == CALL_FUNCION)
    {
        construir_funcion_call(nodo->der, instrucciones);
        mov->arg1 = buscar_resultado(instrucciones);
    }
    else
    {
        construir_op(nodo->der, instrucciones);
        mov->arg1 = buscar_resultado(instrucciones);
    }
    insertar_cuadruplo(mov, instrucciones);
}

void construir_iteracion(Arbol *nodo, Instrucciones *instrucciones)
{
    if (nodo->izq)
    {
        Cuadruplo *start_while = malloc(sizeof(Cuadruplo));
        start_while->op = TAG;
        start_while->resultado = crear_simbolo(NULL, ETIQUETA);
        insertar_cuadruplo(start_while, instrucciones);
        construir_op(nodo->izq, instrucciones);
        // Salto condicional para saltear el while.
        Cuadruplo *jumpc = malloc(sizeof(Cuadruplo));
        jumpc->op = JMPC;
        jumpc->arg1 = buscar_resultado(instrucciones);
        jumpc->resultado = crear_simbolo(NULL, ETIQUETA);
        insertar_cuadruplo(jumpc, instrucciones);

        if (nodo->der)
        {
            generar_codigo(nodo->der, instrucciones);
            Cuadruplo *ciclo = malloc(sizeof(Cuadruplo));
            ciclo->op = JMP;
            ciclo->resultado = start_while->resultado;
            insertar_cuadruplo(ciclo, instrucciones);
        }
        Cuadruplo *end_while = malloc(sizeof(Cuadruplo));
        end_while->op = TAG;
        end_while->resultado = jumpc->resultado;
        insertar_cuadruplo(end_while, instrucciones);
    }
}

void construir_funcion_decl(Arbol *arbol, Instrucciones *instrucciones)
{
    Cuadruplo *start_fun = malloc(sizeof(Cuadruplo));
    start_fun->op = START_FUN;
    insertar_cuadruplo(start_fun, instrucciones);
    Cuadruplo *tag = malloc(sizeof(Cuadruplo));
    tag->op = TAG;
    tag->resultado = crear_simbolo(NULL, ETIQUETA);
    insertar_cuadruplo(tag, instrucciones);
    generar_codigo(arbol->izq, instrucciones);
    Cuadruplo *end_fun = malloc(sizeof(Cuadruplo));
    end_fun->op = END_FUN;
    insertar_cuadruplo(end_fun, instrucciones);
}

void construir_params(Arbol *nodo, Instrucciones *instrucciones)
{
    Parametro_Call *params_call = nodo->info->funcion_call.params;
    while (params_call != NULL)
    {
        Cuadruplo *param = malloc(sizeof(Cuadruplo));
        param->op = PARAM;
        generar_codigo(params_call->expr, instrucciones);
        param->arg1 = buscar_resultado(instrucciones);
        insertar_cuadruplo(param, instrucciones);
        param = NULL;
        params_call = params_call->next;
    }
}

void construir_funcion_call(Arbol *arbol, Instrucciones *instrucciones)
{
    construir_params(arbol, instrucciones);
    Cuadruplo *call = malloc(sizeof(Cuadruplo));
    call->op = CALL;
    call->arg1 = cant_params(arbol);
    call->resultado = crear_simbolo(arbol->info, ETIQUETA);
    insertar_cuadruplo(call, instrucciones);
}