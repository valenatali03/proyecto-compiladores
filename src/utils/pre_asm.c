#include "../includes/pre_asm.h"
#include "string.h"

int CANT_LINEAS = 0;
int CANT_TEMP = 0;
int CANT_JUMP = 0;
int CANT_TAG = 0;
int OFFSET = 0;
int OFFSET_INC = 4;
int OFFSET_PARAMS_REG = 0;
int OFFSET_PARAMS = 0;
int OFFSET_GAP = 16;
int CANT_VAR = 0;
char **codigo = NULL;
Instrucciones *instrucciones = NULL;
Instrucciones *ultima_instruccion = NULL;
Temporales *temporales_libres = NULL;

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
        CANT_VAR += 1;
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
    ret->arg1 = NULL;
    ret->arg2 = NULL;
    ret->resultado = NULL;
    if (!nodo->izq)
    {
        insertar_cuadruplo(ret, instrucciones);
        return;
    }

    ret->arg1 = construir_expresion(nodo->izq, instrucciones);
    actualizar_temp(ret->arg1);

    insertar_cuadruplo(ret, instrucciones);
}

Simbolo *obtener_arg(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return NULL;

    switch (nodo->tipo_info)
    {
    case OPERADOR_UNARIO:
    case OPERADOR_BINARIO:
        return construir_op(nodo, instrucciones);
    case ID:
    case LITERAL:
        return crear_simbolo(nodo->info, nodo->tipo_info);
    case CALL_FUNCION:
        return construir_funcion_call(nodo, instrucciones);
    default:
        return NULL;
    }
}

Simbolo *construir_op(Arbol *nodo, Instrucciones *instrucciones)
{
    if (nodo == NULL)
        return NULL;

    if (nodo->tipo_info == OPERADOR_BINARIO)
    {
        Cuadruplo *cuad = malloc(sizeof(Cuadruplo));
        cuad->op = traducir_op(nodo->info->operador.nombre);
        cuad->arg1 = obtener_arg(nodo->izq, instrucciones);
        cuad->arg2 = obtener_arg(nodo->der, instrucciones);
        actualizar_temp(cuad->arg1);
        actualizar_temp(cuad->arg2);
        cuad->resultado = crear_simbolo(NULL, ID);
        insertar_cuadruplo(cuad, instrucciones);
        return (cuad->resultado);
    }

    else if (nodo->tipo_info == OPERADOR_UNARIO)
    {
        Cuadruplo *cuad = malloc(sizeof(Cuadruplo));
        cuad->op = traducir_op(nodo->info->operador.nombre);
        cuad->arg1 = obtener_arg(nodo->izq, instrucciones);
        cuad->arg2 = NULL;
        actualizar_temp(cuad->arg1);
        cuad->resultado = crear_simbolo(NULL, ID);
        insertar_cuadruplo(cuad, instrucciones);
        return (cuad->resultado);
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
    if (!nodo->medio)
        return;

    Cuadruplo *cuad = malloc(sizeof(Cuadruplo));
    cuad->op = IF_FALSE;
    cuad->arg1 = construir_expresion(nodo->medio, instrucciones);
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
        end_if->arg1 = NULL;
        end_if->arg2 = NULL;
        insertar_cuadruplo(end_if, instrucciones);
        // Construyo el cuadruplo que contiene la etiqueta del else.
        Cuadruplo *els = malloc(sizeof(Cuadruplo));
        els->op = TAG;
        els->arg1 = NULL;
        els->arg2 = NULL;
        els->resultado = cuad->resultado;
        insertar_cuadruplo(els, instrucciones);
        // Genero el codigo del else
        construir_bloque(nodo->der, instrucciones);
        // Construyo el cuadruplo que contiene el fin del else
        Cuadruplo *end_else = malloc(sizeof(Cuadruplo));
        end_else->op = TAG;
        end_else->arg1 = NULL;
        end_else->arg2 = NULL;
        end_else->resultado = end_if->resultado;
        insertar_cuadruplo(end_else, instrucciones);
    }
    else
    {
        // if sin else
        Cuadruplo *end_if = malloc(sizeof(Cuadruplo));
        end_if->op = TAG;
        end_if->arg1 = NULL;
        end_if->arg2 = NULL;
        end_if->resultado = cuad->resultado;
        insertar_cuadruplo(end_if, instrucciones);
    }
}

void construir_asignacion(Arbol *nodo, Instrucciones *instrucciones)
{
    Cuadruplo *mov = malloc(sizeof(Cuadruplo));
    mov->op = MOV;
    mov->arg1 = construir_expresion(nodo->der, instrucciones);
    actualizar_temp(mov->arg1);
    mov->arg2 = NULL;
    mov->resultado = crear_simbolo(nodo->izq->info, nodo->izq->tipo_info);
    insertar_cuadruplo(mov, instrucciones);
}

void construir_iteracion(Arbol *nodo, Instrucciones *instrucciones)
{
    if (nodo->izq)
    {
        Cuadruplo *start_while = malloc(sizeof(Cuadruplo));
        start_while->op = TAG;
        start_while->arg1 = NULL;
        start_while->arg2 = NULL;
        start_while->resultado = crear_etiqueta(NULL);
        insertar_cuadruplo(start_while, instrucciones);
        // Salto condicional para saltear el while.
        Cuadruplo *jumpc = malloc(sizeof(Cuadruplo));
        jumpc->op = JMPC;
        jumpc->arg1 = construir_expresion(nodo->izq, instrucciones);
        jumpc->arg2 = NULL;
        jumpc->resultado = crear_etiqueta(NULL);
        insertar_cuadruplo(jumpc, instrucciones);

        if (nodo->der)
        {
            construir_bloque(nodo->der, instrucciones);
            Cuadruplo *ciclo = malloc(sizeof(Cuadruplo));
            ciclo->op = JMP;
            ciclo->arg1 = NULL;
            ciclo->arg2 = NULL;
            ciclo->resultado = start_while->resultado;
            insertar_cuadruplo(ciclo, instrucciones);
        }
        Cuadruplo *end_while = malloc(sizeof(Cuadruplo));
        end_while->op = TAG;
        end_while->arg1 = NULL;
        end_while->arg2 = NULL;
        end_while->resultado = jumpc->resultado;
        insertar_cuadruplo(end_while, instrucciones);
    }
}

void construir_funcion_decl(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo->info->funcion_decl.esExterna)
    {
        CANT_VAR = 0;
        Cuadruplo *tag = malloc(sizeof(Cuadruplo));
        tag->op = TAG;
        tag->arg1 = NULL;
        tag->arg2 = NULL;
        tag->resultado = crear_etiqueta(nodo->info->funcion_decl.nombre);
        insertar_cuadruplo(tag, instrucciones);
        Cuadruplo *start_fun = malloc(sizeof(Cuadruplo));
        start_fun->op = START_FUN;
        start_fun->arg1 = crear_simbolo(nodo->info, nodo->tipo_info);
        start_fun->arg2 = NULL;
        start_fun->resultado = NULL;
        insertar_cuadruplo(start_fun, instrucciones);

        construir_bloque(nodo->izq, instrucciones);
        nodo->info->funcion_decl.cantVariables = CANT_VAR;
        OFFSET = nodo->info->funcion_decl.cantVariables * OFFSET_INC;
        nodo->info->funcion_decl.offset = OFFSET;
        Cuadruplo *end_fun = malloc(sizeof(Cuadruplo));
        end_fun->op = END_FUN;
        end_fun->arg1 = NULL;
        end_fun->arg2 = NULL;
        end_fun->resultado = NULL;
        insertar_cuadruplo(end_fun, instrucciones);
    }
    else
    {
        Cuadruplo *externo = malloc(sizeof(Cuadruplo));
        externo->op = EXTERN;
        externo->arg1 = crear_simbolo(nodo->info, nodo->tipo_info);
        externo->arg2 = NULL;
        externo->resultado = NULL;
        insertar_cuadruplo(externo, instrucciones);
    }
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

void construir_params(Parametro_Call *params_call, Instrucciones *instrucciones, Instrucciones *parametros)
{
    if (!params_call)
    {
        return;
    }
    if (params_call)
    {
        construir_params(params_call->next, instrucciones, parametros);
    }

    Cuadruplo *param = malloc(sizeof(Cuadruplo));
    param->op = PARAM;
    param->arg1 = construir_expresion(params_call->expr, instrucciones);
    param->arg2 = NULL;
    param->resultado = NULL;
    insertar_cuadruplo(param, parametros);
}
Simbolo *construir_funcion_call(Arbol *nodo, Instrucciones *instrucciones)
{
    Parametro_Call *params = nodo->info->funcion_call.params;

    if (params)
    {
        Instrucciones *parametros = crear_lista_instrucciones();
        construir_params(params, instrucciones, parametros);
        actualizar_temp_params(parametros);
        insertar_cuadruplos(parametros, instrucciones);
    }

    Cuadruplo *call = malloc(sizeof(Cuadruplo));
    call->op = CALL;
    Info_Union *info = malloc(sizeof(Info_Union));
    int *cant = malloc(sizeof(int));
    *cant = nodo->info->funcion_call.cant_params;
    info->literal.valor = cant;
    info->literal.tipo = ENTERO;
    call->arg1 = crear_simbolo(info, LITERAL);
    call->arg2 = crear_etiqueta(nodo->info->funcion_call.nombre);
    call->resultado = crear_simbolo(NULL, ID);
    insertar_cuadruplo(call, instrucciones);
    return call->resultado;
}

void insertar_cuadruplos(Instrucciones *p, Instrucciones *q)
{
    Instrucciones *aux = p;
    while (aux)
    {
        insertar_cuadruplo(aux->expr, q);
        aux = aux->next;
    }
}

Simbolo *construir_expresion(Arbol *nodo, Instrucciones *instrucciones)
{
    if (!nodo)
        return NULL;

    switch (nodo->tipo_info)
    {
    case OPERADOR_UNARIO:
    case OPERADOR_BINARIO:
        return construir_op(nodo, instrucciones);
        break;
    case ID:
    case LITERAL:
        return crear_simbolo(nodo->info, nodo->tipo_info);
    case CALL_FUNCION:
        return construir_funcion_call(nodo, instrucciones);
        break;
    default:
        return NULL;
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

void actualizar_temp(Simbolo *temp)
{
    if (!temp)
        return;

    if (temp->flag != ID)
        return;

    if (!temp->info->id.temp)
        return;

    if (!temporales_libres)
    {
        temporales_libres = malloc(sizeof(Temporales));
        temporales_libres->head = temp;
        temp->next = NULL;
        return;
    }

    temp->next = temporales_libres->head;
    temporales_libres->head = temp;
}

void actualizar_temp_params(Instrucciones *params)
{
    if (!params)
    {
        return;
    }

    Instrucciones *aux = params;

    while (aux)
    {
        actualizar_temp(aux->expr->arg1);
        aux = aux->next;
    }
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

    Simbolo *s = malloc(sizeof(Simbolo));
    s->flag = flag;
    if (flag == ID)
    {
        char buffer[16];
        if (!info)
        {

            if (temporales_libres)
            {
                s = temporales_libres->head;
                if (!temporales_libres->head->next)
                {
                    temporales_libres = NULL;
                }
                else
                {
                    temporales_libres->head = temporales_libres->head->next;
                }

                return s;
            }

            Info_Union *info = malloc(sizeof(Info_Union));
            sprintf(buffer, "t%d", CANT_TEMP++);
            char *n = strdup(buffer);
            s->flag = flag;
            info->id.nombre = n;
            info->id.offset = -1;
            info->id.global = 0;
            info->id.temp = 1;
            s->info = info;
            CANT_VAR += 1;

            return s;
        }
        else
        {
            s->info = info;

            return s;
        }
    }

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

void simbolo_a_str(Simbolo *s, char buffer[64])
{
    if (s == NULL)
    {
        buffer[0] = '\0';
        return;
    }

    switch (s->flag)
    {
    case ID:
        sprintf(buffer, "%s", s->info->id.nombre);
        break;

    case LITERAL:
        switch (s->info->literal.tipo)
        {
        case ENTERO:
            sprintf(buffer, "%d", *((int *)s->info->literal.valor));
            break;
        case BOOL:
            sprintf(buffer, "%s", *((int *)s->info->literal.valor) ? "true" : "false");
            break;
        default:
            sprintf(buffer, "<?>");
            break;
        }
        break;

    case DECL_FUNCION:
        sprintf(buffer, "%s", s->info->funcion_decl.nombre);
        break;

    case CALL_FUNCION:
        sprintf(buffer, "%s", s->info->funcion_call.nombre);
        break;

    case ETIQUETA:
        sprintf(buffer, "%s", s->info->etiqueta.nombre);
        break;

    default:
        sprintf(buffer, "<?>");
        break;
    }
}

void instrucciones_to_str(Instrucciones *instrucciones)
{
    Instrucciones *aux = instrucciones;
    char buffer[256];

    CANT_LINEAS = 0;
    free(codigo);
    codigo = NULL;
    char res[64], a1[64], a2[64];
    char *op;

    while (aux != NULL)
    {
        Cuadruplo *cuad = aux->expr;

        op = tipo_op_str[cuad->op];
        simbolo_a_str(cuad->resultado, res);
        simbolo_a_str(cuad->arg1, a1);
        simbolo_a_str(cuad->arg2, a2);

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
