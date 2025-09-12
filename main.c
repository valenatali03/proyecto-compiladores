#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;

// Archivo de salida global
FILE *out;

void imprimir_uso(const char *prog) {
    fprintf(stderr, "Uso: %s [-target etapa] archivo.ctds\n", prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        imprimir_uso(argv[0]);
    }

    char *target = "parse"; // Por defecto ejecuta hasta la etapa parse
    char *output = NULL;
    char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-target") == 0) {
            if (++i >= argc) imprimir_uso(argv[0]);
            target = argv[i];
        } else {
            // Código fuente del programa a compilar
            filename = argv[i];
        }
    }

    // Validación del archivo
    if (!filename || filename[0] == '-' || !strstr(filename, ".ctds")) {
        fprintf(stderr, "Error: archivo inválido. Debe terminar en .ctds y no comenzar con '-'\n");
        return 1;
    }

    yyin = fopen(filename, "r");
    if (!yyin) {
        perror("No se puede abrir archivo");
        return 1;
    }

    // Definir nombre de salida por defecto
    if (!output) {
        char *dot = strrchr(filename, '.');
        size_t len = dot - filename;
        output = malloc(len + 10);
        strncpy(output, filename, len);
        output[len] = '\0';

        if (strcmp(target, "scan") == 0) {
            strcat(output, ".lex");
        } else if (strcmp(target, "parse") == 0) {
            strcat(output, ".sint");
        } else {
            fprintf(stderr, "Target desconocido: %s\n", target);
            fclose(yyin);
            return 1;
        }
    }

    // Abrir archivo de salida global
    out = fopen(output, "w");
    if (!out) {
        perror("No se puede crear archivo de salida");
        fclose(yyin);
        return 1;
    }

    // Ejecución según target
    if (strcmp(target, "scan") == 0) {
        while (yylex() != 0) {}
    } else if (strcmp(target, "parse") == 0) {
        yyparse();
    }

    fclose(yyin);
    fclose(out);

    return 0;
}
