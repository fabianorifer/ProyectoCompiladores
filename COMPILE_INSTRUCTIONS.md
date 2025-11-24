# Instrucciones de Compilación y Ejecución

## 🔨 Compilación del Scanner

### Opción 1: Compilar solo el scanner de prueba
```bash
g++ -o test_scanner.exe test_scanner.cpp scanner.cpp token.cpp -std=c++11
```

### Opción 2: Compilar el proyecto completo
```bash
g++ -o a.exe main.cpp scanner.cpp token.cpp parser.cpp ast.cpp visitor.cpp -std=c++11
```

---

## 🚀 Ejecución

### Probar el scanner con un solo archivo:
```bash
./test_scanner.exe inputs/BaseInputs5/input1.txt
```
Esto generará un archivo `input1_tokens.txt` con los tokens escaneados.

### Ejecutar todos los inputs con el script Python:
```bash
python run_all_inputs.py
```

El script te mostrará un menú:
```
============================================================
📁 Carpetas disponibles en inputs/:
============================================================
  1. BaseInputs5 (5 archivos)
  2. ExtensionsInputs5 (5 archivos)
  3. FunctionInputs3 (3 archivos)
  4. OptimizationInputs5 (5 archivos)
  5. Ejecutar TODAS las carpetas
  0. Salir
============================================================
```

---

## 📂 Estructura de Outputs

Después de ejecutar, los outputs se organizan así:

```
outputs/
├── BaseInputs5/
│   ├── scanner_outputs/
│   │   ├── input1_tokens.txt
│   │   ├── input2_tokens.txt
│   │   └── ...
│   └── (archivos .s si se generan)
├── ExtensionsInputs5/
│   └── scanner_outputs/
│       └── ...
└── ...
```

---

## 📊 Interpretación de Resultados

### Scanner exitoso:
```
Scanner

TOKEN(LET, "let")
TOKEN(ID, "x")
TOKEN(COLON, ":")
TOKEN(I32, "i32")
...
TOKEN(END)

Scanner exitoso
```

### Scanner no exitoso:
```
Scanner

TOKEN(LET, "let")
TOKEN(ERR, "@")
Caracter invalido

Scanner no exitoso
```

---

## 🧪 Verificar Tokens de un Input Específico

1. Ejecuta el scanner:
```bash
./test_scanner.exe inputs/BaseInputs5/input1.txt
```

2. Revisa el archivo generado:
```bash
cat inputs/BaseInputs5/input1_tokens.txt
```

O con PowerShell:
```powershell
Get-Content inputs/BaseInputs5/input1_tokens.txt
```

---

## 📝 Tokens Soportados

Ver `SCANNER_ANALYSIS.md` para la lista completa de tokens soportados.

**Resumen rápido:**
- ✅ Tipos: i32, i64, f32, f64, bool
- ✅ Keywords: let, mut, fn, return, if, else, while, for, in, as
- ✅ Operadores: +, -, *, /, %, **, ==, !=, <, <=, >, >=, &&, ||, !
- ✅ Asignación: =, +=, -=, *=, /=, %=
- ✅ Punteros: *mut, *const, &, &mut
- ✅ Literales: enteros, flotantes, strings, true, false
- ✅ Delimitadores: (, ), {, }, ;, ,, :, ->, ..

---

## ⚠️ Solución de Problemas

### Error: "g++ no reconocido"
Necesitas instalar MinGW o usar un compilador C++ compatible.

### Alternativa con MSVC (Visual Studio):
```cmd
cl /EHsc test_scanner.cpp scanner.cpp token.cpp
```

### El script Python no encuentra archivos:
Asegúrate de estar en la carpeta raíz del proyecto:
```bash
cd c:\Users\lopez_dd7jjab\Documents\Ciclo_2025_2\Compiladores\ProyectoCompiladores
```
