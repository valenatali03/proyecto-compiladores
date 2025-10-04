#include "../../includes/pre_asm.h"
#include "string.h"

void agregar_instrucciones(Instrucciones *destino, Instrucciones *origen)
{
    if (!origen)
        return;
    Instrucciones *aux = destino;
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    aux->next = origen;
}

void generar_codigo(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;

    Instrucciones *decl = crear_lista_instrucciones();
    Instrucciones *stmt = crear_lista_instrucciones();

    if (nodo->izq)
    {
        decl = construir_declaraciones(nodo->izq);
        agregar_instrucciones(instrucciones, decl);
    }
    if (nodo->der)
    {
        stmt = construir_sentencias(nodo->der);
        agregar_instrucciones(instrucciones, stmt);
    }
}

Instrucciones *construir_declaraciones(Arbol *nodo)
{
    if (nodo == NULL)
        return NULL;

    Instrucciones *decl = crear_lista_instrucciones();
    switch (nodo->tipo_info)
    {
    case DECL_FUNCION:
        construir_funcion_decl(nodo, decl);
        break;

    case DECLARACION_VARIABLE:
        construir_asignacion(nodo, decl);
        break;

    default:
        Instrucciones *izq = construir_declaraciones(nodo->izq);
        Instrucciones *der = construir_declaraciones(nodo->der);
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

Instrucciones *construir_sentencias(Arbol *nodo)
{
    if (nodo == NULL)
        return NULL;

    Instrucciones *stmt = crear_lista_instrucciones();
    switch (nodo->tipo_info)
    {
    case CALL_FUNCION:
        construir_funcion_call(nodo, stmt);
        break;
    case ASIGNACION:
        construir_asignacion(nodo, stmt);
        break;
    case IF:
        construir_condicional(nodo, stmt);
        break;
    case WHILE:
        construir_iteracion(nodo, stmt);
        break;
    case RETURN:
        construir_return(nodo, stmt);
        break;
    default:
        Instrucciones *izq = construir_sentencias(nodo->izq);
        Instrucciones *der = construir_sentencias(nodo->der);
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
    Tipo_Info flag = nodo->izq->tipo_info;

    switch (flag)
    {
    case ID:
    case LITERAL:
        ret->arg1 = crear_simbolo(nodo->izq->info, flag);
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

        case LITERAL:
        case ID:
            cuad->arg1 = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
            break;

        case CALL_FUNCION:
            construir_funcion_call(nodo->izq, instrucciones);
            cuad->arg1 = buscar_resultado(instrucciones);
            break;

        default:
            break;
        }
        switch (nodo->der->tipo_info)
        {
        case OPERADOR_BINARIO:
            cuad->arg2 = buscar_resultado(instrucciones);
            break;

        case LITERAL:
        case ID:
            cuad->arg2 = crear_simbolo(nodo->der->info, nodo->der->tipo_info);
            break;

        case CALL_FUNCION:
            construir_funcion_call(nodo->der, instrucciones);
            cuad->arg2 = buscar_resultado(instrucciones);
            break;

        default:
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

        case LITERAL:
        case ID:
            cuad->arg1 = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
            break;

        case CALL_FUNCION:
            construir_funcion_call(nodo->izq, instrucciones);
            cuad->arg1 = buscar_resultado(instrucciones);
            break;

        default:
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
    if (strcmp(op, "%") == 0)
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
    return NOP;
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
    cuad->resultado = crear_etiqueta(NULL);
    insertar_cuadruplo(cuad, instrucciones);
    generar_codigo(nodo->izq, instrucciones);
    if (nodo->der)
    {
        // Construyo el cuadruplo para saltar al final si pasé por el bloque verdadero.
        Cuadruplo *end_if = malloc(sizeof(Cuadruplo));
        end_if->op = JMP;
        end_if->resultado = crear_etiqueta(NULL);
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
    mov->resultado = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
    Tipo_Info flag_der = nodo->der->tipo_info;

    switch (flag_der)
    {
    case LITERAL:
    case ID:
        mov->arg1 = crear_simbolo(nodo->der->info, flag_der);
        break;

    case CALL_FUNCION:
        construir_funcion_call(nodo->der, instrucciones);
        mov->arg1 = buscar_resultado(instrucciones);
        break;

    case OPERADOR_UNARIO:
    case OPERADOR_BINARIO:
        construir_op(nodo->der, instrucciones);
        mov->arg1 = buscar_resultado(instrucciones);
        break;

    default:
        break;
    }

    insertar_cuadruplo(mov, instrucciones);
}

void construir_iteracion(Arbol *nodo, Instrucciones *instrucciones)
{
    if (nodo->izq)
    {
        Cuadruplo *start_while = malloc(sizeof(Cuadruplo));
        start_while->op = TAG;
        start_while->resultado = crear_etiqueta(NULL);
        insertar_cuadruplo(start_while, instrucciones);
        construir_op(nodo->izq, instrucciones);
        // Salto condicional para saltear el while.
        Cuadruplo *jumpc = malloc(sizeof(Cuadruplo));
        jumpc->op = JMPC;
        jumpc->arg1 = buscar_resultado(instrucciones);
        jumpc->resultado = crear_etiqueta(NULL);
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

void construir_funcion_decl(Arbol *nodo, Instrucciones *instrucciones)
{
    Cuadruplo *start_fun = malloc(sizeof(Cuadruplo));
    start_fun->op = START_FUN;
    insertar_cuadruplo(start_fun, instrucciones);
    Cuadruplo *tag = malloc(sizeof(Cuadruplo));
    tag->op = TAG;
    tag->resultado = crear_etiqueta(NULL);
    insertar_cuadruplo(tag, instrucciones);
    generar_codigo(nodo->izq, instrucciones);
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

void construir_funcion_call(Arbol *nodo, Instrucciones *instrucciones)
{
    construir_params(nodo, instrucciones);
    Cuadruplo *call = malloc(sizeof(Cuadruplo));
    call->op = CALL;
    call->arg1 = cant_params(nodo);
    call->resultado = crear_etiqueta(nodo->info->funcion_call.nombre);
    insertar_cuadruplo(call, instrucciones);
}

void insertar_cuadruplo(Cuadruplo *c, Instrucciones *inst)
{
    if (!c || !inst)
        return;
    Instrucciones *aux = inst;
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    Instrucciones *nuevo = malloc(sizeof(Instrucciones));
    nuevo->expr = c;
    nuevo->next = NULL;
    aux->next = nuevo;
}

Simbolo *buscar_resultado(Instrucciones *inst)
{
    if (!inst)
        return NULL;
    Instrucciones *aux = inst;
    Simbolo *res = NULL;
    while (aux != NULL)
    {
        if (aux->expr->op != TAG)
        {
            res = aux->expr->resultado;
        }
        aux = aux->next;
    }
    return res;
}

int cant_params(Arbol *nodo)
{
    int count = 0;
    Parametro_Call *params = nodo->info->funcion_call.params;
    while (params)
    {
        count++;
        params = params->next;
    }
    return count;
}

Simbolo *crear_etiqueta(char *nombre)
{
    char buffer[16];
    Simbolo *s = malloc(sizeof(Simbolo));
    Info_Etiqueta *info = malloc(sizeof(Info_Etiqueta));
    if (nombre == NULL)
    {
        sprintf(buffer, "L%d", CANT_TAG++);
        char *n = strdup(buffer);
        info->nombre = n;
    }
    else
    {
        info->nombre = nombre;
    }
    s->flag = ETIQUETA;
    s->info = info;
    return s;
}

Instrucciones *crear_lista_instrucciones()
{
    Instrucciones *lista = malloc(sizeof(Instrucciones));
    lista->expr = NULL;
    lista->next = NULL;
    return lista;
}
