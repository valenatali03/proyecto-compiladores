# Nombre del ejecutable
TARGET = scanner

# Directorios
INCLUDES = includes
SRC = src/utils
BUILD = build

# Archivos fuente
LEXER = scanner.l
PARSER = parser.y
SRCS = $(SRC)/ast.c $(SRC)/params.c

# Herramientas
LEX = flex
YACC = bison
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -I$(INCLUDES)

# Archivos generados por flex/bison
LEX_C = lex.yy.c
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h

# Objetos (en build/)
OBJS = $(BUILD)/ast.o $(BUILD)/params.o $(BUILD)/parser.tab.o $(BUILD)/lex.yy.o

# Regla principal
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

# Compilación de objetos del src/
$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# parser.tab.o
$(BUILD)/parser.tab.o: $(PARSER_C) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# lex.yy.o
$(BUILD)/lex.yy.o: $(LEX_C) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Generar parser.tab.c y parser.tab.h desde parser.y
$(PARSER_C) $(PARSER_H): $(PARSER)
	$(YACC) -d $<

# Generar lex.yy.c desde scanner.l
$(LEX_C): $(LEXER) $(PARSER_H)
	$(LEX) $<

# Crear carpeta build si no existe
$(BUILD):
	mkdir -p $(BUILD)

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(LEX_C) $(PARSER_C) $(PARSER_H) $(BUILD)/*.o
