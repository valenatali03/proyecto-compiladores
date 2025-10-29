#include "../../includes/opt_operadores.h"
void optimizar_operaciones(Arbol *arbol)
{
    if (arbol != NULL)
    {
        if (arbol->izq != NULL)
        {
            switch (arbol->izq->tipo_info)
            {
            case OPERADOR_BINARIO:
            case OPERADOR_UNARIO:
                arbol->izq = reducir_operador(arbol->izq);
                break;

            default:
                optimizar_operaciones(arbol->izq);
                break;
            }
        }

        if (arbol->der != NULL)
        {
            switch (arbol->der->tipo_info)
            {
            case OPERADOR_BINARIO:
            case OPERADOR_UNARIO:
                arbol->der = reducir_operador(arbol->der);
                break;

            default:
                optimizar_operaciones(arbol->der);
                break;
            }
        }

        if (arbol->medio != NULL)
        {
            switch (arbol->medio->tipo_info)
            {
            case OPERADOR_BINARIO:
            case OPERADOR_UNARIO:
                arbol->medio = reducir_operador(arbol->medio);
                break;

            default:
                optimizar_operaciones(arbol->medio);
                break;
            }
        }
    }
}

Arbol *reducir_operador(Arbol *nodo)
{
    if (nodo != NULL)
    {
        void *a;
        void *b;
        void *res;
        switch (nodo->tipo_info)
        {
        case OPERADOR_BINARIO:
            nodo->der = reducir_operador(nodo->der);
            nodo->izq = reducir_operador(nodo->izq);
            if (nodo->izq->tipo_info == LITERAL && nodo->der->tipo_info == LITERAL)
            {

                a = nodo->izq->info->literal.valor;
                b = nodo->der->info->literal.valor;

                res = realizar_operacion_binaria(a, b, nodo);
                if (res != NULL)
                {
                    Tipo tipo_resultado = nodo->info->operador.tipo;
                    Arbol *nuevo_nodo = crear_arbol_literal(res, tipo_resultado, nodo->linea, nodo->colum, NULL, NULL);

                    liberar_nodo_arbol(nodo->izq);
                    liberar_nodo_arbol(nodo->der);
                    liberar_nodo_arbol(nodo);

                    return nuevo_nodo;
                }
            }
            break;

        case OPERADOR_UNARIO:
            nodo->izq = reducir_operador(nodo->izq);
            if (nodo->izq->tipo_info == LITERAL)
            {
                a = nodo->izq->info->literal.valor;
                res = realizar_operacion_unaria(a, nodo);
                if (res != NULL)
                {
                    Tipo tipo_resultado = nodo->info->operador.tipo;
                    Arbol *nuevo_nodo = crear_arbol_literal(res, tipo_resultado, nodo->linea, nodo->colum, NULL, NULL);

                    liberar_nodo_arbol(nodo->izq);
                    liberar_nodo_arbol(nodo);

                    return nuevo_nodo;
                }
            }
            break;
        default:
            return nodo;
            break;
        }
    }

    return nodo;
}

void *realizar_operacion_binaria(void *a, void *b, Arbol *nodo_operador)
{

    Info_Operador op_info = nodo_operador->info->operador;
    void *resultado = NULL;

    if (op_info.tipo == ENTERO)
    {
        int val_a = *(int *)a;
        int val_b = *(int *)b;

        if (strcmp(op_info.nombre, "+") == 0)
        {
            resultado = malloc(sizeof(int));
            *(int *)resultado = val_a + val_b;
        }
        else if (strcmp(op_info.nombre, "-") == 0)
        {
            resultado = malloc(sizeof(int));
            *(int *)resultado = val_a - val_b;
        }
        else if (strcmp(op_info.nombre, "*") == 0)
        {
            resultado = malloc(sizeof(int));
            *(int *)resultado = val_a * val_b;
        }
        else if (strcmp(op_info.nombre, "/") == 0)
        {
            if (val_b == 0)
            {
                printf("Linea %d Col %d └── Error: Se encontró una división por cero.\n", nodo_operador->linea, nodo_operador->colum);
                exit(1);
            }
            resultado = malloc(sizeof(int));
            *(int *)resultado = val_a / val_b;
        }
        else if (strcmp(op_info.nombre, "%") == 0)
        {
            resultado = malloc(sizeof(int));
        }

        else if (strcmp(op_info.nombre, "==") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = (val_a == val_b);
        }
        else if (strcmp(op_info.nombre, "!=") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = (val_a != val_b);
        }
        else if (strcmp(op_info.nombre, "<") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = (val_a < val_b);
        }
        else if (strcmp(op_info.nombre, ">") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = (val_a > val_b);
        }
    }
    else if (op_info.tipo == BOOL)
    {

        bool val_a = *(bool *)a;
        bool val_b = *(bool *)b;

        if (strcmp(op_info.nombre, "&&") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = val_a && val_b;
        }
        else if (strcmp(op_info.nombre, "||") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = val_a || val_b;
        }
        else if (strcmp(op_info.nombre, "==") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = val_a == val_b;
        }
        else if (strcmp(op_info.nombre, "!=") == 0)
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = val_a != val_b;
        }
    }

    return resultado;
}

void *realizar_operacion_unaria(void *a, Arbol *nodo_operador)
{

    Info_Operador op_info = nodo_operador->info->operador;
    void *resultado = NULL;

    if (op_info.tipo == ENTERO)
    {
        int val_a = *(int *)a;
        if (strcmp(op_info.nombre, "-") == 0)
        {
            resultado = malloc(sizeof(int));
            *(int *)resultado = -1 * val_a;
        }
    }
    else if (op_info.tipo == BOOL)
    {

        bool val_a = *(bool *)a;
        if (strcmp(op_info.nombre, "!" == 0))
        {
            resultado = malloc(sizeof(bool));
            *(bool *)resultado = !val_a;
        }
    }
    return resultado;
}

void liberar_nodo_arbol(Arbol *nodo)
{
    if (nodo == NULL)
    {
        return;
    }

    if (nodo->info != NULL)
    {
        if (nodo->tipo_info == LITERAL)
        {
            if (nodo->info->literal.valor != NULL)
            {
                free(nodo->info->literal.valor);
                nodo->info->literal.valor = NULL;
            }
        }

        if (nodo->tipo_info == OPERADOR_BINARIO || nodo->tipo_info == OPERADOR_UNARIO)
        {
            free(nodo->info->operador.nombre);
        }

        if (nodo->tipo_info == ID)
        {
            free(nodo->info->id.nombre);
            free(nodo->info->id.valor);
        }
        free(nodo->info);
        nodo->info = NULL;
    }

    free(nodo);
}
