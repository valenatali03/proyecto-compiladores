# Taller de Diseño de Software

**Código:** 3306  
**Proyecto**  
**Año:** 2025  

## Integrantes
- Campregher Bruno  
- Machuca Franco  
- Natali Valentino

## Compilación y ejecución
Para compilar el proyecto, ejecutar el comando `make` desde el directorio raiz del proyecto.
Se crearán los siguientes archivos:
  - c-tds
  - lex.yy.c
  - parser.tab.c
  - parser.tab.h

El comando para ejecutar el programa es: 
`./c-tds -target (TARGET) (NOMBRE_PROGRAMA).ctds (-opt|-debug)?`
Donde `NOMBRE_PROGRAMA` es el nombre del archivo de entrada y `TARGET` especifica hasta qué etapa se ejecutará. Se aceptan los siguientes valores para `TARGET`:
  - `scan` (Scanner)
  - `parse` (Parser)
  - `ast` (Arbol Sintáctico y Tabla de Símbolos)
  - `sem` (Análisis Semántico)
  - `ci` (Generación de Código Intermedio)
  - `s` (Generación de Código Assembler)

### Argumentos opcionales
  - -opt: Ejecuta las siguientes optimizaciones en las etapas de Análisis Semántico, Generación de Código Intermedio y Código Assembler:
      - Propagación de constantes
      - Eliminación de código muerto
      - Reutilización de variables temporales
      - Optimización de offsets
      - Eliminación de declaraciones no utilizadas
  
  - -debug: Genera archivos adicionales como:
      - Imagenes de los AST (optimizado y sin optimizar) (Requiere la librería **Graphviz** instalada)
      - Archivo .ci con el código intermedio

## Tests
El comando para ejecutar los tests para todas las etapas es: `./run_tests.sh`. Se informará por consola cuántos de los tests fueron exitosos (tanto válidos como inválidos) para cada etapa.

## Ramas correspondientes a entregas de etapas
  - Entrega 1 (Scanner y Parser): Entrega-15/9
  - Entrega 2 (AST y Tabla de Símbolos): Entrega-24/9
  - Entrega 3 (Análisis Semántico): Entrega-01/10
  - Entrega 4 (Código Intermedio): Entrega-08/10
  - Entrega 5 (Código Objeto): Entrega-27/10
  - Entrega 6 (Optimizaciones): main
