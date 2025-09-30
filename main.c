#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "includes/ast.h"
#include "includes/tsim.h"
#include "includes/semantico.h"
#include "includes/errores.h"

extern int yylex(void);
extern int yyparse(Arbol **arbol);
extern FILE *yyin;

// Archivos de salida globales
FILE *out_lex = NULL;  // Para analizador léxico
FILE *out_sint = NULL; // Para analizador sintáctico
FILE *out_sem = NULL;  // Para analizador semántico

void imprimir_uso(const char *prog)
{
    fprintf(stderr, "Uso: %s [-target etapa] archivo.ctds\n", prog);
    exit(1);
}

void imprimir_errores()
{
    for (int i = 0; i < cantErrores; i++)
    {
        fprintf(out_sem, "Error: %s\n", tipo_err_str[errores[i].codigo]);
        fprintf(out_sem, "%s\n", errores[i].mensaje);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        imprimir_uso(argv[0]);
    }

    char *target = "ast"; // Por defecto ejecuta hasta la etapa ast
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

    // Abrir archivos de salida según target
    if (strcmp(target, "scan") == 0)
    {
        char fname[300];
        snprintf(fname, sizeof(fname), "%s.lex", base);
        out_lex = fopen(fname, "w");
        if (!out_lex)
        {
            perror("No se puede crear archivo .lex");
            fclose(yyin);
            return 1;
        }
    }
    else if (strcmp(target, "parse") == 0)
    {
        char fname1[300], fname2[300];
        snprintf(fname1, sizeof(fname1), "%s.lex", base);  // Archivo de salida de análisis léxico
        snprintf(fname2, sizeof(fname2), "%s.sint", base); // Archivo de salida de análisis sintáctico
        out_lex = fopen(fname1, "w");
        out_sint = fopen(fname2, "w");
        if (!out_lex || !out_sint)
        {
            perror("No se pueden crear archivos de salida");
            fclose(yyin);
            if (out_lex)
                fclose(out_lex);
            if (out_sint)
                fclose(out_sint);
            return 1;
        }
    }
    else if (strcmp(target, "ast") == 0)
    {
        char fname1[300], fname2[300];
        snprintf(fname1, sizeof(fname1), "%s.lex", base);  // Archivo de salida de análisis léxico
        snprintf(fname2, sizeof(fname2), "%s.sint", base); // Archivo de salida de análisis sintáctico
        out_lex = fopen(fname1, "w");
        out_sint = fopen(fname2, "w");
        if (!out_lex || !out_sint)
        {
            perror("No se pueden crear archivos de salida");
            fclose(yyin);
            if (out_lex)
                fclose(out_lex);
            if (out_sint)
                fclose(out_sint);
            return 1;
        }
    }
    else if (strcmp(target, "sem") == 0)
    {
        char fname1[300], fname2[300], fname3[300];
        snprintf(fname1, sizeof(fname1), "%s.lex", base);  // Archivo de salida de análisis léxico
        snprintf(fname2, sizeof(fname2), "%s.sint", base); // Archivo de salida de análisis sintáctico
        snprintf(fname3, sizeof(fname3), "%s.sem", base);  // Archivo de salida de análisis semántico
        out_lex = fopen(fname1, "w");
        out_sint = fopen(fname2, "w");
        out_sem = fopen(fname3, "w");
        if (!out_lex || !out_sint || !out_sem)
        {
            perror("No se pueden crear archivos de salida");
            fclose(yyin);
            if (out_lex)
                fclose(out_lex);
            if (out_sint)
                fclose(out_sint);
            if (out_sem)
            {
                fclose(out_sem);
            }

            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Target desconocido: %s\n", target);
        fclose(yyin);
        return 1;
    }

    // Ejecución según target
    if (strcmp(target, "scan") == 0)
    {
        while (yylex() != 0)
        {
        }
    }
    else if (strcmp(target, "parse") == 0)
    {
        Arbol *arbol = NULL;
        int res = yyparse(&arbol); // Ejecuta el parser
        if (res == 0)
        {
            fprintf(out_sint, "Análisis sintáctico exitoso\n");
        }
        else
        {
            fprintf(out_sint, "Se detectaron errores de sintaxis\n");
        }
    }
    else if (strcmp(target, "ast") == 0)
    {
        Arbol *arbol = NULL;
        int res = yyparse(&arbol); // Ejecuta el parser
        if (res == 0)
        {
            fprintf(out_sint, "Análisis sintáctico exitoso\n");
            exportar_ast_a_dot(arbol, "arbol.dot");
        }
        else
        {
            fprintf(out_sint, "Se detectaron errores de sintaxis\n");
        }
    }
    else if (strcmp(target, "sem") == 0)
    {
        Arbol *arbol = NULL;
        int res = yyparse(&arbol); // Ejecuta el parser
        if (res == 0)
        {
            fprintf(out_sint, "Análisis sintáctico exitoso\n");
            exportar_ast_a_dot(arbol, "arbol.dot");
            tabla = crear_tabla();
            Nivel *nivelActual = tabla;
            analisis_semantico(arbol, nivelActual);
            imprimir_errores();
        }
        else
        {
            fprintf(out_sint, "Se detectaron errores de sintaxis\n");
        }
    }

    // Cerrar archivos
    fclose(yyin);
    if (out_lex)
        fclose(out_lex);
    if (out_sint)
        fclose(out_sint);
    if (out_sem)
        fclose(out_sem);

    return 0;
}
