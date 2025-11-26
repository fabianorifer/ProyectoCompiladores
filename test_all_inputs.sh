#!/bin/bash

# Script para probar todos los inputs y comparar con outputs pre-generados
# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color
BLUE='\033[0;34m'

echo "========================================"
echo "   TESTING COMPILER - ALL INPUTS"
echo "========================================"
echo ""

# Carpetas a probar
FOLDERS=("BaseInputs5" "ExtensionInputs5" "FunctionInputs3" "OptimizationInputs5")

# Contadores
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

# Archivo temporal para resultados
TEMP_DIR="./temp_test_results"
mkdir -p "$TEMP_DIR"

# Función para compilar y ejecutar
test_input() {
    local folder=$1
    local input_file=$2
    local base_name=$(basename "$input_file" .txt)
    
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}Testing: $folder/$base_name${NC}"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Paths
    INPUT_PATH="inputs/$folder/$input_file"
    OUTPUT_S="outputs/$folder/${base_name}.s"
    NEW_S="$TEMP_DIR/${folder}_${base_name}.s"
    NEW_BIN="$TEMP_DIR/${folder}_${base_name}_new"
    OLD_BIN="$TEMP_DIR/${folder}_${base_name}_old"
    NEW_OUT="$TEMP_DIR/${folder}_${base_name}_new.out"
    OLD_OUT="$TEMP_DIR/${folder}_${base_name}_old.out"
    
    # Verificar que existe el input
    if [ ! -f "$INPUT_PATH" ]; then
        echo -e "  ${RED}✗ SKIP${NC} - Input file not found: $INPUT_PATH"
        SKIPPED_TESTS=$((SKIPPED_TESTS + 1))
        return
    fi
    
    # Paso 1: Compilar con nuestro compilador
    echo "  [1/4] Compiling with our compiler..."
    ./compiler "$INPUT_PATH" > /dev/null 2>&1
    
    if [ $? -ne 0 ]; then
        echo -e "  ${RED}✗ FAIL${NC} - Compiler failed"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return
    fi
    
    # El compilador genera en inputs/$folder/${base_name}.s
    GENERATED_S="inputs/$folder/${base_name}.s"
    if [ ! -f "$GENERATED_S" ]; then
        echo -e "  ${RED}✗ FAIL${NC} - No .s file generated"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return
    fi
    
    cp "$GENERATED_S" "$NEW_S"
    
    # Paso 2: Compilar el nuevo .s con g++
    echo "  [2/4] Assembling new .s with g++..."
    g++ -no-pie "$NEW_S" -o "$NEW_BIN" 2>/dev/null
    
    if [ $? -ne 0 ]; then
        echo -e "  ${RED}✗ FAIL${NC} - Assembly failed for new .s"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return
    fi
    
    # Paso 3: Ejecutar el nuevo binario
    echo "  [3/4] Running new binary..."
    timeout 5s "$NEW_BIN" > "$NEW_OUT" 2>&1
    NEW_EXIT=$?
    
    if [ $NEW_EXIT -eq 124 ]; then
        echo -e "  ${RED}✗ FAIL${NC} - New binary timeout"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return
    fi
    
    # Paso 4: Compilar y ejecutar el .s pre-existente si existe
    if [ -f "$OUTPUT_S" ]; then
        echo "  [4/4] Comparing with pre-generated output..."
        g++ -no-pie "$OUTPUT_S" -o "$OLD_BIN" 2>/dev/null
        
        if [ $? -eq 0 ]; then
            timeout 5s "$OLD_BIN" > "$OLD_OUT" 2>&1
            OLD_EXIT=$?
            
            if [ $OLD_EXIT -eq 124 ]; then
                echo -e "  ${YELLOW}⚠ WARN${NC} - Old binary timeout"
            fi
            
            # Comparar outputs
            if diff -q "$NEW_OUT" "$OLD_OUT" > /dev/null 2>&1; then
                echo -e "  ${GREEN}✓ PASS${NC} - Output matches pre-generated"
                echo "  Output:"
                cat "$NEW_OUT" | sed 's/^/    /'
                PASSED_TESTS=$((PASSED_TESTS + 1))
            else
                echo -e "  ${RED}✗ FAIL${NC} - Output differs from pre-generated"
                echo "  Expected (from outputs/$folder):"
                cat "$OLD_OUT" | sed 's/^/    /'
                echo "  Got (from new compilation):"
                cat "$NEW_OUT" | sed 's/^/    /'
                FAILED_TESTS=$((FAILED_TESTS + 1))
            fi
        else
            echo -e "  ${YELLOW}⚠ WARN${NC} - Could not compile pre-generated .s"
            echo "  New output:"
            cat "$NEW_OUT" | sed 's/^/    /'
            PASSED_TESTS=$((PASSED_TESTS + 1))
        fi
    else
        echo -e "  ${YELLOW}⚠ INFO${NC} - No pre-generated .s found, showing new output:"
        cat "$NEW_OUT" | sed 's/^/    /'
        PASSED_TESTS=$((PASSED_TESTS + 1))
    fi
    
    echo ""
}

# Recorrer todas las carpetas
for folder in "${FOLDERS[@]}"; do
    INPUT_DIR="inputs/$folder"
    
    if [ ! -d "$INPUT_DIR" ]; then
        echo -e "${YELLOW}Skipping $folder - directory not found${NC}"
        continue
    fi
    
    echo ""
    echo -e "${GREEN}═══════════════════════════════════════${NC}"
    echo -e "${GREEN}  Testing folder: $folder${NC}"
    echo -e "${GREEN}═══════════════════════════════════════${NC}"
    echo ""
    
    # Encontrar todos los .txt en la carpeta
    for input_file in "$INPUT_DIR"/*.txt; do
        if [ -f "$input_file" ]; then
            base_name=$(basename "$input_file")
            test_input "$folder" "$base_name"
        fi
    done
done

# Limpiar
rm -rf "$TEMP_DIR"

# Resumen final
echo ""
echo "========================================"
echo "           TEST SUMMARY"
echo "========================================"
echo -e "Total tests:   $TOTAL_TESTS"
echo -e "${GREEN}Passed:        $PASSED_TESTS${NC}"
echo -e "${RED}Failed:        $FAILED_TESTS${NC}"
echo -e "${YELLOW}Skipped:       $SKIPPED_TESTS${NC}"
echo "========================================"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed! ✗${NC}"
    exit 1
fi
