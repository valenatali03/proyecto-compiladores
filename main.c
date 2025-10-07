#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "includes/ast.h"
#include "includes/tsim.h"
#include "includes/semantico.h"
#include "includes/errores.h"
#include "includes/pre_asm.h"

extern int yylex(void);
extern int yyparse(Arbol **arbol);
extern FILE *yyin;

// Archivos de salida globales
FILE *out_lex = NULL;  // Para analizador léxico
FILE *out_sint = NULL; // Para analizador sintáctico
FILE *out_sem = NULL;  // Para analizador semántico
FILE *out_ci = NULL;   // Para codigo intermedio

typedef enum
{
    TARGET_SCAN,
    TARGET_PARSE,
    TARGET_AST,
    TARGET_SEM,
    TARGET_CI,
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
    if (out_lex)
    {
        fclose(out_lex);
        out_lex = NULL;
    }
    if (out_sint)
    {
        fclose(out_sint);
        out_sint = NULL;
    }
    if (out_sem)
    {
        fclose(out_sem);
        out_sem = NULL;
    }
    if (out_ci)
    {
        fclose(out_ci);
        out_ci = NULL;
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

    char *target = "sem"; // Por defecto ejecuta hasta la etapa sem
    char *filename = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-target") == 0)
        {
            if (++i >= argc)
                imprimir_uso(argv[0]);
            target = argv[i];
        }
        else
        {
            filename = argv[i]; // Archivo de entrada
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

    // Abrir archivos de salida

    switch (t)
    {
    case TARGET_SCAN:
        out_lex = abrir_archivo(base, ".lex");
        if (!out_lex)
        {
            cerrar_archivos();
            return 1;
        }
        break;

    case TARGET_PARSE:
    case TARGET_AST:
        out_lex = abrir_archivo(base, ".lex");
        out_sint = abrir_archivo(base, ".sint");
        if (!out_lex || !out_sint)
        {
            cerrar_archivos();
            return 1;
        }
        break;

    case TARGET_SEM:
        out_lex = abrir_archivo(base, ".lex");
        out_sint = abrir_archivo(base, ".sint");
        out_sem = abrir_archivo(base, ".sem");
        if (!out_lex || !out_sint || !out_sem)
        {
            cerrar_archivos();
            return 1;
        }
        break;

    case TARGET_CI:
        out_lex = abrir_archivo(base, ".lex");
        out_sint = abrir_archivo(base, ".sint");
        out_sem = abrir_archivo(base, ".sem");
        out_ci = abrir_archivo(base, ".ci");
        if (!out_lex || !out_sint || !out_sem || !out_ci)
        {
            cerrar_archivos();
            return 1;
        }
        break;

    default:
        fprintf(stderr, "Target desconocido: %s\n", target);
        return 1;
    }

    // Ejecutar etapa

    if (t == TARGET_SCAN)
    {
        while (yylex() != 0)
        {
        }
        cerrar_archivos();
        return 0;
    }

    Arbol *arbol = NULL;
    int res = yyparse(&arbol);

    if (res != 0)
    {
        return 1;
    }

    fprintf(out_sint, "Análisis sintáctico exitoso\n");

    if (t == TARGET_AST || t == TARGET_SEM || t == TARGET_CI)
    {
        exportar_ast_a_dot(arbol, "arbol.dot");
    }

    if (t == TARGET_SEM || t == TARGET_CI)
    {
        tabla = crear_tabla();
        Nivel *nivelActual = tabla;
        analisis_semantico(arbol, nivelActual);
        int error_semantico = reportar_resultado_semantico(out_sem);

        if (t == TARGET_CI && !error_semantico)
        {
            instrucciones = crear_lista_instrucciones();
            generar_codigo(arbol, instrucciones);
            instrucciones_to_str(instrucciones);
            imprimir_codigo_ci(out_ci);
        }
    }

    cerrar_archivos();

    return 0;
}
