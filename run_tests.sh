#!/bin/bash

echo "=== Ejecutando Tests Scan ==="
for dir in valid invalid; do
    echo "--- $dir ---"
    for file in tests/scan/$dir/*.ctds; do
        echo "Scan: $file"
        ./c-tds -target scan "$file"
    done
done
echo ""

echo "=== Ejecutando Tests Parser ==="
for dir in valid invalid; do
    echo "--- $dir ---"
    for file in tests/parser/$dir/*.ctds; do
        echo "Parse: $file"
        ./c-tds -target parse "$file"
    done
done

echo "=== Ejecutando Tests Semantica ==="
for dir in valid invalid; do
    echo "--- $dir ---"
    find tests/semantica/$dir -type f -name "*.ctds" | while read file; do
        echo "Semantica: $file"
        ./c-tds -target sem "$file"
    done
done

echo "=== Fin de tests ==="
