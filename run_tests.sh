#!/bin/bash

# Ejecuta tests de análisis sintáctico
run_sintaxis() {
    echo "=== Sintaxis ==="
    for dir in valid invalid; do
        ok=0 fail=0
        for file in tests/parser/$dir/*.ctds; do
            nombre=$(basename "$file")
            ./c-tds -target parse "$file" &>/dev/null
            exitcode=$?
            if ([ "$dir" = "valid" ] && [ $exitcode -eq 0 ]) || ([ "$dir" = "invalid" ] && [ $exitcode -ne 0 ]); then
                ((ok++))
            else
                echo "FAIL: $nombre"
                ((fail++))
            fi
        done
        echo "$dir: $ok/$((ok+fail))"
    done
    echo ""
}

# Ejecuta tests de análisis semántico
run_semantica() {
    echo "=== Semántica ==="
    for dir in valid invalid; do
        ok=0 fail=0
        for file in tests/semantica/$dir/**/*.ctds; do
            nombre=$(basename "$file")
            ./c-tds -target sem "$file" &>/dev/null
            exitcode=$?
            if ([ "$dir" = "valid" ] && [ $exitcode -eq 0 ]) || ([ "$dir" = "invalid" ] && [ $exitcode -ne 0 ]); then
                ((ok++))
            else
                echo "FAIL: $nombre"
                ((fail++))
            fi
        done
        echo "$dir: $ok/$((ok+fail))"
    done
    echo ""
}

# Ejecuta tests de generación de código assembler
run_assembler() {
    echo "=== Assembler ==="
    echo ""
    
    for modo in "" "-opt"; do
        [ -z "$modo" ] && echo "Sin optimizaciones" || echo "Con optimizaciones"
        
        ok=0
        fail=0
        for f in tests/assembler/*.ctds; do
            base="${f%.ctds}"
            nombre=$(basename "$base")
            
            # Genera código assembler (archivo .s)
            ./c-tds -target s $modo "$f"
            
            # Compila funciones externas y archivo generado
            gcc -c tests/assembler/externas.c -o externas.o
            gcc -c "$base.s" -o "$base.o"
            gcc "$base.o" externas.o -o "$base.out" 2>/dev/null
            
            # Ejecuta y compara con salida esperada
            salida=$("$base.out")
            esperado=$(cat "$base.cmp")
            
            if [ "$salida" = "$esperado" ]; then
                ((ok++))
            else
                echo "FAIL: $nombre${modo:+ (opt)}"
                echo "  Esperado: $esperado"
                echo "  Obtenido: $salida"
                ((fail++))
            fi
            
            # Limpiar archivos temporales
            rm -f "$base.s" "$base.o" externas.o "$base.out"
        done
        
        [ $fail -eq 0 ] && echo "All pass ($ok)" || echo "$ok PASS, $fail FAIL"
        echo ""
    done
}


echo ""
echo "Ejecución de tests"
echo ""

if [ -n "$1" ]; then
    run_$1 2>/dev/null || { echo "Uso: $0 [sintaxis|semantica|assembler]"; exit 1; }
else
    run_sintaxis
    run_semantica
    run_assembler
fi