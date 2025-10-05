#include "../includes/pre_asm.h"
#include "string.h"

int CANT_LINEAS = 0;
int CANT_TEMP = 0;
int CANT_JUMP = 0;
int CANT_TAG = 0;
char **codigo = NULL;
Instrucciones *instrucciones = NULL;

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

    if (nodo->izq)
    {
        construir_declaracion_variables(nodo->izq, instrucciones);
    }
    if (nodo->der)
    {
        construir_declaracion_metodos(nodo->der, instrucciones);
    }
}

void construir_declaracion_variables(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;

    if (nodo->izq)
    {
        construir_declaracion_variables(nodo->izq, instrucciones);
    }

    if (nodo->der)
    {
        construir_declaracion_variables(nodo->der, instrucciones);
    }

    if (nodo->tipo_info == DECLARACION_VARIABLE)
    {
        construir_asignacion(nodo, instrucciones);
    }
}

void construir_declaracion_metodos(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;

    if (nodo->tipo_info == DECL_FUNCION)
    {
        construir_funcion_decl(nodo, instrucciones);
        return;
    }

    if (nodo->izq)
    {
        construir_declaracion_metodos(nodo->izq, instrucciones);
    }

    if (nodo->der)
    {
        construir_declaracion_metodos(nodo->der, instrucciones);
    }
}

void construir_sentencias(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;

    if (nodo->izq)
    {
        construir_sentencia(nodo->izq, instrucciones);
    }

    if (nodo->der)
    {
        construir_sentencia(nodo->der, instrucciones);
    }
}

void construir_sentencia(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;

    switch (nodo->tipo_info)
    {
    case CALL_FUNCION:
        construir_funcion_call(nodo, instrucciones);
        break;
    case ASIGNACION:
        construir_asignacion(nodo, instrucciones);
        break;
    case IF:
        construir_condicional(nodo, instrucciones);
        break;
    case WHILE:
        construir_iteracion(nodo, instrucciones);
        break;
    case RETURN:
        construir_return(nodo, instrucciones);
        break;
    case SENTENCIAS:
        construir_sentencias(nodo, instrucciones);
        break;
    case BLOQUE:
        construir_bloque(nodo, instrucciones);
    default:
        break;
    }
}

void construir_return(Arbol *nodo, Instrucciones *instrucciones)
{
    Cuadruplo *ret = malloc(sizeof(Cuadruplo));
    ret->op = RET;
    if (!nodo->izq)
    {
        insertar_cuadruplo(ret, instrucciones);
        return;
    }

    Tipo_Info flag = nodo->izq->tipo_info;

    switch (flag)
    {
    case ID:
    case LITERAL:
        ret->arg1 = crear_simbolo(nodo->izq->info, flag);
        break;
    case OPERADOR_BINARIO:
    case OPERADOR_UNARIO:
    case CALL_FUNCION:
        construir_expresion(nodo->izq, instrucciones);
        ret->arg1 = buscar_resultado(instrucciones);
        break;
    default:
        break;
    }

    insertar_cuadruplo(ret, instrucciones);
}

Simbolo *obtener_arg(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo) return NULL;

    switch (nodo->tipo_info)
    {
    case OPERADOR_UNARIO:
    case OPERADOR_BINARIO:
        return buscar_resultado(instrucciones);
    case ID:
    case LITERAL:
        return crear_simbolo(nodo->info, nodo->tipo_info);
    case CALL_FUNCION:
        construir_funcion_call(nodo, instrucciones);
        return buscar_resultado(instrucciones);
    default:
        return NULL;
    }
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

    if (nodo->tipo_info == OPERADOR_BINARIO) {
        cuad->op = traducir_op(nodo->info->operador.nombre);
        cuad->arg1 = obtener_arg(nodo->izq, instrucciones);
        cuad->arg2 = obtener_arg(nodo->der, instrucciones);
        cuad->resultado = crear_simbolo(NULL, ID);
        insertar_cuadruplo(cuad, instrucciones);
    }
    else if (nodo->tipo_info == OPERADOR_UNARIO)
    {
        cuad->op = traducir_op(nodo->info->operador.nombre);
        cuad->arg1 = obtener_arg(nodo->izq, instrucciones);
        cuad->resultado = crear_simbolo(NULL, ID);
        insertar_cuadruplo(cuad, instrucciones);
    }
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
        construir_expresion(nodo->medio, instrucciones);
    }

    Cuadruplo *cuad = malloc(sizeof(Cuadruplo));
    cuad->op = IF_FALSE;

    if (nodo->medio->tipo_info == ID || nodo->medio->tipo_info == LITERAL)
    {
        cuad->arg1 = crear_simbolo(nodo->medio->info, nodo->medio->tipo_info);
    }
    else
    {
        cuad->arg1 = buscar_resultado(instrucciones);
    }

    cuad->arg2 = NULL;
    // Etiqueta para saltear el bloque verdadero
    cuad->resultado = crear_etiqueta(NULL);
    insertar_cuadruplo(cuad, instrucciones);
    construir_bloque(nodo->izq, instrucciones);
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
        construir_bloque(nodo->der, instrucciones);
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
    case OPERADOR_UNARIO:
    case OPERADOR_BINARIO:
        construir_expresion(nodo->der, instrucciones);
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
        construir_expresion(nodo->izq, instrucciones);
        // Salto condicional para saltear el while.
        Cuadruplo *jumpc = malloc(sizeof(Cuadruplo));
        jumpc->op = JMPC;
        if (nodo->izq->tipo_info == ID || nodo->izq->tipo_info == LITERAL)
        {
            jumpc->arg1 = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
        }
        else
        {
            jumpc->arg1 = buscar_resultado(instrucciones);
        }

        jumpc->resultado = crear_etiqueta(NULL);
        insertar_cuadruplo(jumpc, instrucciones);

        if (nodo->der)
        {
            construir_bloque(nodo->der, instrucciones);
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
    construir_bloque(nodo->izq, instrucciones);
    Cuadruplo *end_fun = malloc(sizeof(Cuadruplo));
    end_fun->op = END_FUN;
    insertar_cuadruplo(end_fun, instrucciones);
}

void construir_bloque(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;
    if (nodo->izq)
    {
        construir_declaracion_variables(nodo->izq, instrucciones);
    }

    if (nodo->der)
    {
        construir_sentencias(nodo->der, instrucciones);
    }
}

void construir_params(Arbol *nodo, Instrucciones *instrucciones)
{
    Parametro_Call *params_call = nodo->info->funcion_call.params;
    while (params_call != NULL)
    {
        Cuadruplo *param = malloc(sizeof(Cuadruplo));
        param->op = PARAM;
        construir_expresion(params_call->expr, instrucciones);
        if (params_call->expr->tipo_info == LITERAL || params_call->expr->tipo_info == ID)
        {
            param->arg1 = crear_simbolo(params_call->expr->info, params_call->expr->tipo_info);
        }
        else
        {
            param->arg1 = buscar_resultado(instrucciones);
        }

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
    Info_Union *info = malloc(sizeof(Info_Union));
    int *cant = malloc(sizeof(int));
    *cant = cant_params(nodo);
    info->literal.valor = cant;
    info->literal.tipo = ENTERO;
    call->arg1 = crear_simbolo(info, LITERAL);
    call->resultado = crear_etiqueta(nodo->info->funcion_call.nombre);
    insertar_cuadruplo(call, instrucciones);
}

void construir_expresion(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return;

    switch (nodo->tipo_info)
    {
    case OPERADOR_UNARIO:
    case OPERADOR_BINARIO:
        construir_op(nodo, instrucciones);
        break;

    case CALL_FUNCION:
        construir_funcion_call(nodo, instrucciones);
        break;
    default:
        break;
    }
}

void insertar_cuadruplo(Cuadruplo *c, Instrucciones *inst)
{
    if (!c || !inst)
        return;

    if (inst->expr == NULL)
    {
        inst->expr = c;
        return;
    }

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
    Info_Union *info = malloc(sizeof(Info_Union));
    if (nombre == NULL)
    {
        sprintf(buffer, "L%d", CANT_TAG++);
        char *n = strdup(buffer);
        info->etiqueta.nombre = n;
    }
    else
    {
        info->etiqueta.nombre = nombre;
    }
    s->flag = ETIQUETA;
    s->info = info;
    return s;
}

Simbolo *crear_simbolo(Info_Union *info, Tipo_Info flag)
{
    if (!info)
    {
        char buffer[16];
        Simbolo *s = malloc(sizeof(Simbolo));
        Info_Union *info = malloc(sizeof(Info_Union));
        sprintf(buffer, "t%d", CANT_TEMP++);
        char *n = strdup(buffer);
        s->flag = flag;
        info->id.nombre = n;
        s->info = info;

        return s;
    }

    Simbolo *s = malloc(sizeof(Simbolo));
    s->flag = flag;
    s->info = info;
    s->next = NULL;

    return s;
}

Instrucciones *crear_lista_instrucciones()
{
    Instrucciones *lista = malloc(sizeof(Instrucciones));
    lista->expr = NULL;
    lista->next = NULL;
    return lista;
}

static char *simbolo_a_str(Simbolo *s)
{
    if (s == NULL)
        return "";

    static char buffer[64];

    switch (s->flag)
    {
    case ID:
        return s->info->id.nombre;

    case LITERAL:
        switch (s->info->literal.tipo)
        {
        case ENTERO:
            sprintf(buffer, "%d", *((int *)s->info->literal.valor));
            return buffer;
        case BOOL:
            sprintf(buffer, "%s", *((int *)s->info->literal.valor) ? "true" : "false");
            return buffer;
        default:
            return "<?>"; // tipo literal no manejado
        }

    case CALL_FUNCION:
        return s->info->funcion_call.nombre;

    case ETIQUETA:
        return s->info->etiqueta.nombre;

    default:
        return "<?>"; // tipo de símbolo no manejado
    }
}

void instrucciones_to_str(Instrucciones *instrucciones)
{
    Instrucciones *aux = instrucciones;
    char buffer[256];

    CANT_LINEAS = 0;
    free(codigo);
    codigo = NULL;

    while (aux != NULL)
    {
        Cuadruplo *cuad = aux->expr;

        char *res = cuad->resultado ? simbolo_a_str(cuad->resultado) : "";
        char *a1 = cuad->arg1 ? simbolo_a_str(cuad->arg1) : "";
        char *a2 = cuad->arg2 ? simbolo_a_str(cuad->arg2) : "";
        char *op = tipo_op_str[cuad->op];

        switch (cuad->op)
        {
        case NOT:
            sprintf(buffer, "%s %s %s", op, a1, res);
            break;

        case MOV:
            sprintf(buffer, "%s %s %s", op, a1, res);
            break;

        case TAG:
            sprintf(buffer, "%s:", res);
            break;

        case MINUS:
            if (strcmp(a2, "") == 0)
            {
                sprintf(buffer, "%s %s %s", op, a1, res);
                break;
            }

        default:
            sprintf(buffer, "%s %s %s %s", op, a1, a2, res);
            break;
        }

        codigo = realloc(codigo, (CANT_LINEAS + 1) * sizeof(char *));
        codigo[CANT_LINEAS] = strdup(buffer);
        CANT_LINEAS++;

        aux = aux->next;
    }
}

void imprimir_codigo_ci(FILE *out_ci)
{
    for (int i = 0; i < CANT_LINEAS; i++)
    {
        fprintf(out_ci, "%s\n", codigo[i]);
    }
}
