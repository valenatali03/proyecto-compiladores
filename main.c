#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "includes/ast.h"
#include "includes/tsim.h"
#include "includes/semantico.h"
#include "includes/errores.h"
#include "includes/asm.h"
#include "includes/opt_operadores.h"
#include "includes/opt_codigo_muerto.h"

extern int yylex(void);
extern int yyparse(Arbol **arbol);
extern FILE *yyin;

// Archivos de salida globales

FILE *out_ci = NULL; // Para código intermedio
FILE *out_s = NULL;  // Para código assembler

typedef enum
{
    TARGET_SCAN,
    TARGET_PARSE,
    TARGET_AST,
    TARGET_SEM,
    TARGET_CI,
    TARGET_S,
    TARGET_DESCONOCIDO
} Target;

Target parse_target(const char *target_str)
{
    if (strcmp(target_str, "scan") == 0)
        return TARGET_SCAN;
    if (strcmp(target_str, "parse") == 0)
        return TARGET_PARSE;
    if (strcmp(target_str, "ast") == 0)
        return TARGET_AST;
    if (strcmp(target_str, "sem") == 0)
        return TARGET_SEM;
    if (strcmp(target_str, "ci") == 0)
        return TARGET_CI;
    if (strcmp(target_str, "s") == 0)
        return TARGET_S;
    return TARGET_DESCONOCIDO;
}

void imprimir_uso(const char *prog)
{
    fprintf(stderr, "Uso: %s [-target etapa] archivo.ctds\n", prog);
    exit(1);
}

FILE *abrir_archivo(const char *base, const char *ext)
{
    char fname[300];
    snprintf(fname, sizeof(fname), "%s%s", base, ext);
    FILE *f = fopen(fname, "w");
    if (!f)
    {
        perror("No se puede crear archivo de salida");
        return NULL;
    }
    return f;
}

void cerrar_archivos()
{
    if (out_ci)
    {
        fclose(out_ci);
        out_ci = NULL;
    }
    if (out_s)
    {
        fclose(out_s);
        out_s = NULL;
    }
    if (yyin)
    {
        fclose(yyin);
        yyin = NULL;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        imprimir_uso(argv[0]);
    }

    char *target = "s"; // Por defecto ejecuta hasta la etapa s
    char *filename = NULL;

    bool debug_info = false;
    bool optimizar = false;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-target") == 0)
        {
            if (++i >= argc)
            {
                imprimir_uso(argv[0]);
            }
            target = argv[i];
        }
        else if (strcmp(argv[i], "-opt") == 0)
        {
            optimizar = true;
        }
        else if (strcmp(argv[i], "-debug") == 0)
        {
            debug_info = true;
        }
        else
        {
            filename = argv[i]; // archivo de entrada
        }
    }

    // Validación del archivo

    if (!filename || filename[0] == '-' || !strstr(filename, ".ctds"))
    {
        fprintf(stderr, "Error: archivo inválido. Debe terminar en .ctds y no comenzar con '-'\n");
        return 1;
    }

    yyin = fopen(filename, "r");
    if (!yyin)
    {
        perror("No se puede abrir archivo");
        return 1;
    }

    // Base del nombre de salida

    char *dot = strrchr(filename, '.');
    size_t len = dot - filename;
    char base[256];
    strncpy(base, filename, len);
    base[len] = '\0';

    Target t = parse_target(target);

    // Ejecutar etapa

    // Target scan
    if (t == TARGET_SCAN)
    {
        while (yylex() != 0)
        {
        }
        return 0;
    }

    // Creación del arbol
    Arbol *arbol = NULL;
    int res = 0;

    if (t >= TARGET_PARSE)
    {
        res = yyparse(&arbol);
        if (res != 0 || !arbol)
        {
            cerrar_archivos();
            return 1; // Error en el parser
        }
    }

    if (t >= TARGET_AST && debug_info)
    {
        exportar_ast_a_dot(arbol, "arbol_sin_opt.dot");
        system("dot -Tpng arbol_sin_opt.dot -o arbol_sin_opt.png");
        system("rm arbol_sin_opt.dot");
    }

    int error_semantico = 0;

    // Ejecutar análisis semántico

    if (t >= TARGET_SEM)
    {
        tabla = crear_tabla();
        Nivel *nivelActual = tabla;
        analisis_semantico(arbol, nivelActual);
        error_semantico = reportar_resultado_semantico();

        if (error_semantico)
        {
            cerrar_archivos();
            return 1; // Error semántico
        }
    }

    // Ejecutar optimizaciones

    if (optimizar && t >= TARGET_SEM)
    {
        optimizar_operaciones(arbol);
        opt_codigo_muerto(arbol);

        if (debug_info)
        {
            exportar_ast_a_dot(arbol, "arbol_opt.dot");
            system("dot -Tpng arbol_opt.dot -o arbol_opt.png");
            system("rm arbol_opt.dot");
        }
    }

    // Ejecutar generación de código intermedio

    if (t >= TARGET_CI)
    {
        instrucciones = crear_lista_instrucciones();

        if (!instrucciones)
        {
            cerrar_archivos();
            return 1;
        }

        generar_codigo(arbol, instrucciones);
        instrucciones_to_str(instrucciones);

        if (debug_info)
        {
            out_ci = abrir_archivo(base, ".ci");

            if (!out_ci)
            {
                cerrar_archivos();
                return 1;
            }

            imprimir_codigo_ci(out_ci);
        }
    }

    // Ejecutar generación de código assembler

    if (t >= TARGET_S)
    {
        out_s = abrir_archivo(base, ".s");

        if (!out_s)
        {
            cerrar_archivos();
            return 1;
        }

        generar_asm(out_s, instrucciones);
    }

    cerrar_archivos();

    return 0;
}