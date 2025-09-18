#include "../../includes/semantico.h"
#include <stdio.h>
#include <string.h>

extern Nivel* tabla;

void analisis_semantico(Arbol *arbol,Nivel *nivelActual) {
    if (arbol == NULL) return;

    if (arbol->izq) {
        procesar_declaracion_variable(arbol->izq, nivelActual);
    }

    if (arbol->der) {
        procesar_declaracion_metodo(arbol->der, nivelActual);
    }
}

void procesar_declaracion_variable(Arbol *arbol,Nivel *nivelActual) {
    if (arbol == NULL) return;

    if (arbol->izq) {
        procesar_declaracion_variable(arbol->izq, nivelActual);
    }

    if (arbol->der) {
        procesar_declaracion_variable(arbol->der, nivelActual);
    }

    if (arbol->tipo_info == DECLARACION_VARIABLE) {
        declarar_variable(arbol, nivelActual);
    }
}

void declarar_variable(Arbol *arbol,Nivel *nivelActual) {

    if (arbol->izq->info->id.tipo == VACIO) {
        printf("Variable declarada VACIO.\n");
        return;
    }

    char *nombre = arbol->izq->info->id.nombre;

    if (buscar_en_nivel(nivelActual, nombre, arbol->izq->tipo_info)) {
        printf("Variable %s ya declarada.\n", nombre);
        return;
    }

    Arbol *expr = arbol->der;

    if (procesar_expresion(expr, nivelActual)) {
        Tipo tExpr = obtener_tipo(expr, nivelActual);
        Tipo tId = arbol->izq->info->id.tipo;
        if (tId != tExpr) {
            printf("Los tipos NO coinciden en la declaracion.\n");
            return;
        }
    } else {
        printf("Error en la expresion a usar como valor en declaracion\n");
        return;
    }

    agregarSimbolo(nivelActual, arbol->izq->info, arbol->izq->tipo_info);
}

void procesar_declaracion_metodo(Arbol *arbol,Nivel *nivelActual) {
    if (arbol == NULL) return;

    if (arbol->izq) {
        procesar_declaracion_metodo(arbol->izq, nivelActual);
    }

    if (arbol->der) {
        procesar_declaracion_metodo(arbol->der, nivelActual);
    }

    if (arbol->tipo_info == FUNCION_DECL) {
        declarar_metodo(arbol, nivelActual);

        Parametro_Decl *params = arbol->info->funcion_decl.params; // Parámetros formales del método

        if (arbol->izq) {
            nivelActual = procesar_bloque(arbol->izq, nivelActual, params);  
        } else { // Método extern
            if (params) {
                Nivel *temp = agregarNivel(nivelActual);
                procesar_params(params, temp);
                cerrarNivel(temp);
            }
        }
    }
}

void declarar_metodo(Arbol *arbol,Nivel *nivelActual) {

    char *nombre = arbol->info->funcion_decl.nombre;

    if (buscar_en_nivel(nivelActual, nombre, arbol->tipo_info)) {
        printf("Metodo %s ya declarado.\n", nombre);
    } else {
        agregarSimbolo(nivelActual, arbol->info, arbol->tipo_info);
    }
}

void procesar_params(Parametro_Decl *params, Nivel *nivelActual) {
    while (params)
        {
            char *nombre = params->info->id.nombre;

            Tipo t = params->info->id.tipo;

            if (t == VACIO) {
                printf("Parametro no puede ser tipo VACIO\n");
                return;
            }

            if (buscar_en_nivel(nivelActual, nombre, ID)) {
                printf("Variable %s ya declarada.\n", nombre);
                return;
            }

            agregarSimbolo(nivelActual, params->info, ID);

            params = params->next;
        }
}

Nivel *procesar_bloque(Arbol *arbol, Nivel *nivelActual, Parametro_Decl *params) {

    Nivel *nuevo = agregarNivel(nivelActual);

    if (params) {
        procesar_params(params, nuevo);
    }

    if (arbol) {
        if (arbol->izq) {
            procesar_declaracion_variable(arbol->izq, nuevo);
        }

        if (arbol->der) {
            procesar_statements(arbol->der, nuevo);
        }
    }

    return cerrarNivel(nuevo);

}

void procesar_statements(Arbol *arbol, Nivel *nivelActual) {
    if (!arbol) return;

    if (arbol->izq) {
        procesar_statement(arbol->izq, nivelActual);
    }

    if (arbol->der) {
        procesar_statement(arbol->der, nivelActual);
    }
}

int procesar_if(Arbol *arbol, Nivel *nivelActual) {

    if (!arbol) return 0;

    if (!procesar_expresion(arbol->medio, nivelActual)) return 0;

    Tipo t = obtener_tipo(arbol->medio, nivelActual);

    if (t != BOOL) {
        printf("Error de tipo en condición de IF\n");
        return 0;
    }

    procesar_bloque(arbol->izq, nivelActual, NULL);

    if (arbol->der) {
        procesar_bloque(arbol->der, nivelActual, NULL);
    }

    return 1;
}

int procesar_while(Arbol *arbol, Nivel *nivelActual) {
    if (!arbol) return 0;

    if (!procesar_expresion(arbol->izq, nivelActual)) return 0;

    Tipo t = obtener_tipo(arbol->izq, nivelActual);

    if (t != BOOL) {
        printf("Error de tipo en condición de WHILE\n");
        return 0;
    }

    procesar_bloque(arbol->der, nivelActual, NULL);

    return 1;
}

void procesar_statement(Arbol *arbol, Nivel *nivelActual) {
    if (!arbol) return;

    switch (arbol->tipo_info) {
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

int procesar_asignacion(Arbol *arbol, Nivel *nivelActual) {
    if (!arbol) return 0;

    char *nombre = arbol->izq->info->id.nombre;

    Info_Union *simbolo = buscarSimbolo(nivelActual, nombre, arbol->izq->tipo_info);

    if (!simbolo) {
        printf("Asignacion no valida %s no declarada.\n", nombre);
        return 0;
    }

    if (!procesar_expresion(arbol->der, nivelActual)) {
        printf("Error de tipo en expresion de asignacion.\n");
        return 0;
    }

    Tipo tExpr = obtener_tipo(arbol->der, nivelActual);

    Tipo tId = simbolo->id.tipo;

    if (tId != tExpr) {
        printf("Error de tipo en asignacion: %s es de tipo %d pero se asigna tipo %d.\n", nombre, tId, tExpr);
        return 0;
    } 

    arbol->izq->info = simbolo; // Vincular nodo del arbol

    return 1;
}

int procesar_expresion(Arbol *arbol, Nivel *nivelActual) {
    if (!arbol) return 0;

    switch (arbol->tipo_info) {
    case ID:
        char *nombre = arbol->info->id.nombre;

        Info_Union *simbolo = buscarSimbolo(nivelActual, nombre, arbol->tipo_info);

        if (!simbolo) {
            printf("Variable no declarada.\n");
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
        if (arbol->info->literal.tipo == ENTERO) {
            int valor = *(int *)arbol->info->literal.valor;
            if (valor > 2147483647 || valor < -2147483647) {
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

int procesar_operador(Arbol *arbol, Nivel *nivelActual) {
    
    if (!arbol) return 0;

    switch (arbol->tipo_info) {
    case OPERADOR_BINARIO:

        if (!procesar_expresion(arbol->izq, nivelActual) || !procesar_expresion(arbol->der, nivelActual)) return 0;

        Tipo t1 = obtener_tipo(arbol->izq, nivelActual);
        Tipo t2 = obtener_tipo(arbol->der, nivelActual);

        char *op= arbol->info->operador.nombre;

        if (strcmp(op, "==") == 0) {
            return t1 == t2;
        } else if (strcmp(op, ">") == 0 || strcmp(op, "<") == 0) {
            return t1 == ENTERO && t2 == ENTERO;
        } else {
            return t1 == t2 && t1 == arbol->info->operador.tipo;
        }
        break;

    case OPERADOR_UNARIO:
        if (!procesar_expresion(arbol->izq, nivelActual)) return 0;

        Tipo t = obtener_tipo(arbol->izq, nivelActual);

        return t == arbol->info->operador.tipo;
        break;
    default:
        return 0;
        break;
    }

}

Tipo obtener_tipo(Arbol *arbol, Nivel *nivelActual) {
    if (!arbol) return VACIO;

    switch (arbol->tipo_info) {
    case ID:
        return arbol->info->id.tipo;
        break;
    case OPERADOR_BINARIO:
        return arbol->info->operador.tipo;
        break;
    case  OPERADOR_UNARIO:
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

void procesar_return(Arbol *arbol, Nivel *nivelActual) {

    if (!arbol) return;

    Info_Union *metodo = buscarUltimoMetodo(tabla);

    if (!metodo) return;

    Tipo t = metodo->funcion_decl.tipo;

    if (arbol->izq) { //return expr ;
        if (t == VACIO) {
            printf("Un metodo VACIO no debe retornar expresión.\n");
            return;
        }

        if (!procesar_expresion(arbol->izq, nivelActual)) return;

        Tipo tipoExpr = obtener_tipo(arbol->izq, nivelActual);

        if (t != tipoExpr) {
            printf("Error de tipo RETURN\n");
        }
    } else { // return ;
        if (t != VACIO) {
            printf("Error de tipo RETURN\n");
        }
    }
}

int procesar_metodo(Arbol *arbol, Nivel *nivelActual) {
    Info_Union *metodo = buscar_en_nivel(tabla, arbol->info->funcion_call.nombre, FUNCION_DECL);

    if (!metodo) {
        printf("Llamada de método no declarado.\n");
        return 0;
    }

    Parametro_Decl *params_decl = metodo->funcion_decl.params;
    Parametro_Call *params_call = arbol->info->funcion_call.params;

    if (!params_decl && !params_call) {
        return 1;
    }

    if ((!params_decl && params_call) || (params_decl && !params_call)) {
        printf("Error: cantidad de parámetros incorrecta en llamada.\n");
        return 0;
    }

    while (params_decl && params_call) {
        Tipo t1 = params_decl->info->id.tipo;

        if (!procesar_expresion(params_call->expr, nivelActual)) return 0;

        Tipo t2 = obtener_tipo(params_call->expr, nivelActual);

        if (t1 != t2) {
            printf("Los tipos de parametros no coinciden.\n"); 
            return 0;
        }

        params_decl = params_decl->next;
        params_call = params_call->next;
    }

    // Si uno de los dos no terminó al mismo tiempo, error
    if (params_decl || params_call) {
        printf("Cantidad de parámetros no coincide\n");
        return 0;
    }

    return 1;
}
