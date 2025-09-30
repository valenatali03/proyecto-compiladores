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

echo "=== Fin de tests ==="
