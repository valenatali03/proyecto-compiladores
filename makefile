# Nombre del ejecutable
TARGET = c-tds

# Archivos fuente
LEXER = scanner.l
PARSER = parser.y
MAIN = main.c

# Herramientas
LEX = flex
YACC = bison
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -Wno-unused-function

# Archivos generados
LEX_C = lex.yy.c
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h

# Regla principal
$(TARGET): $(PARSER_C) $(LEX_C) $(MAIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(PARSER_C) $(LEX_C) -lfl

# Generar parser.tab.c y parser.tab.h desde parser.y
$(PARSER_C) $(PARSER_H): $(PARSER)
	$(YACC) -d $(PARSER)

# Generar lex.yy.c desde scanner.l
$(LEX_C): $(LEXER) $(PARSER_H)
	$(LEX) $(LEXER)

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(LEX_C) $(PARSER_C) $(PARSER_H) *.lex *.sint
