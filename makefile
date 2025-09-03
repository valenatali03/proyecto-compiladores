# Nombre del ejecutable
TARGET = scanner

# Archivos fuente
LEXER = scanner.l

# Herramientas
LEX = flex
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g

# Archivos generados
LEX_C = lex.yy.c

# Regla principal
$(TARGET): $(LEX_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(LEX_C) -lfl

# Generar lex.yy.c desde scanner.l
$(LEX_C): $(LEXER)
	$(LEX) $(LEXER)

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(LEX_C)
