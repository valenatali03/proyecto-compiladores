# Nombre del ejecutable
TARGET = c-tds

# Directorios
INCLUDES = includes
SRC = src/utils
BUILD = build

# Archivos fuente
LEXER = scanner.l
PARSER = parser.y
SRCS = $(SRC)/ast.c $(SRC)/params.c $(SRC)/enums.c \
	   $(SRC)/tsim.c $(SRC)/semantico.c $(SRC)/errores.c $(SRC)/pre_asm.c $(SRC)/asm.c

# Herramientas
LEX = flex
YACC = bison
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -Wno-unused-function -I$(INCLUDES)

# Archivos generados por flex/bison
LEX_C = lex.yy.c
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h

# Objetos (en build/)
OBJS = $(BUILD)/ast.o $(BUILD)/params.o $(BUILD)/enums.o $(BUILD)/tsim.o \
       $(BUILD)/semantico.o $(BUILD)/errores.o \
	   $(BUILD)/pre_asm.o $(BUILD)/asm.o\
       $(BUILD)/parser.tab.o $(BUILD)/lex.yy.o $(BUILD)/main.o

# Regla principal
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

# Compilación de objetos del src/
$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilación de main.o
$(BUILD)/main.o: main.c | $(BUILD)
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
	rm -f $(TARGET) $(LEX_C) $(PARSER_C) $(PARSER_H) $(BUILD)/*.o *.lex *.sint *.sem *.ci *.s *.png *.dot

