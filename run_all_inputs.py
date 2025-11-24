import os
import subprocess
import shutil
import glob
import sys
import argparse

def get_input_subfolders():
    """Obtiene todas las subcarpetas dentro de 'inputs/'"""
    inputs_dir = "inputs"
    if not os.path.exists(inputs_dir):
        return []
    
    subfolders = []
    for item in os.listdir(inputs_dir):
        item_path = os.path.join(inputs_dir, item)
        if os.path.isdir(item_path):
            subfolders.append(item)
    return sorted(subfolders)

def select_folder():
    """Permite al usuario seleccionar una subcarpeta"""
    subfolders = get_input_subfolders()
    
    if not subfolders:
        print("❌ No se encontraron subcarpetas en 'inputs/'")
        return None
    
    print("\n" + "="*60)
    print("📁 Carpetas disponibles en inputs/:")
    print("="*60)
    for i, folder in enumerate(subfolders, 1):
        folder_path = os.path.join("inputs", folder)
        txt_count = len([f for f in os.listdir(folder_path) if f.endswith('.txt')])
        print(f"  {i}. {folder} ({txt_count} archivos)")
    print(f"  {len(subfolders) + 1}. Ejecutar TODAS las carpetas")
    print("  0. Salir")
    print("="*60)
    
    while True:
        try:
            choice = int(input("\n👉 Selecciona una opción: "))
            if choice == 0:
                print("👋 Saliendo...")
                return None
            elif 1 <= choice <= len(subfolders):
                return subfolders[choice - 1]
            elif choice == len(subfolders) + 1:
                return "ALL"
            else:
                print("❌ Opción inválida. Intenta de nuevo.")
        except ValueError:
            print("❌ Por favor ingresa un número válido.")

def run_inputs_from_folder(subfolder_name, output_dir, run_prefix):
    """Ejecuta todos los inputs de una subcarpeta y mueve archivos .s generados"""
    input_dir = os.path.join("inputs", subfolder_name)
    os.makedirs(output_dir, exist_ok=True)

    input_files = sorted([f for f in os.listdir(input_dir) if f.endswith('.txt')])
    if not input_files:
        print(f"❌ No se encontraron archivos .txt en '{input_dir}'")
        return

    print(f"\n{'='*60}")
    print(f"🚀 Ejecutando {len(input_files)} inputs de: {subfolder_name}")
    print(f"{'='*60}\n")

    success = 0
    fail = 0
    for input_file in input_files:
        filepath = os.path.join(input_dir, input_file)
        # Normalizar separadores si se ejecuta vía WSL
        if run_prefix:
            filepath_exec = filepath.replace('\\', '/')
        else:
            filepath_exec = filepath
        base_name = os.path.splitext(input_file)[0]
        print(f"▶️  Ejecutando {input_file}...", end=" ")
        cmd = run_prefix + ["./a.out", filepath_exec]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode == 0:
            print("✅ OK")
            success += 1
        else:
            print("❌ FALLO (rc=%d)" % result.returncode)
            fail += 1
            if result.stderr.strip():
                print(f"   ⚠️  stderr: {result.stderr.strip()}")
            if result.stdout.strip():
                print(f"   💬 stdout: {result.stdout.strip()}")

        asm_file = os.path.join(input_dir, f"{base_name}.s")
        if os.path.isfile(asm_file):
            shutil.move(asm_file, os.path.join(output_dir, f"{base_name}.s"))

    print(f"\n{'='*60}")
    print(f"📊 RESUMEN - {subfolder_name}")
    print(f"{'='*60}")
    print(f"   ✅ Exitosos: {success}/{len(input_files)}")
    print(f"   ❌ Fallos: {fail}/{len(input_files)}")
    print(f"   📁 ASM en: {output_dir}")
    print(f"{'='*60}\n")

def main():
    parser = argparse.ArgumentParser(description="Ejecuta inputs y genera .s", add_help=True)
    parser.add_argument("--all", action="store_true", help="Ejecutar todas las carpetas de inputs/")
    parser.add_argument("--folder", help="Ejecutar solo una carpeta específica dentro de inputs/")
    args = parser.parse_args()
    programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]
    print("\n" + "="*60)
    print("🔨 COMPILANDO COMPILADOR")
    print("="*60)
    compile = ["g++", "-o", "a.out"] + programa + ["-std=c++11"]
    print("Comando:", " ".join(compile))
    run_prefix = []  # Prefijo para ejecutar (vacío = nativo; ['wsl'] = vía WSL)
    try:
        result = subprocess.run(compile, capture_output=True, text=True)
        if result.returncode != 0:
            print("❌ Error en compilación:\n", result.stderr)
            # Intentar WSL
            print("🔄 Intentando compilar vía WSL...")
            compile_wsl = ["wsl", "g++", "-o", "a.out"] + programa + ["-std=c++11"]
            wsl_result = subprocess.run(compile_wsl, capture_output=True, text=True)
            if wsl_result.returncode != 0:
                print("❌ También falló en WSL:\n", wsl_result.stderr)
                if os.path.exists("a.out"):
                    print("⚠️ Usando binario previo 'a.out' (podría estar desactualizado).")
                else:
                    exit(1)
            else:
                run_prefix = ["wsl"]
                print("✅ Compilación exitosa (WSL)\n")
        else:
            print("✅ Compilación exitosa\n")
    except FileNotFoundError:
        print("⚠️ g++ no encontrado en Windows. Intentando WSL...")
        compile_wsl = ["wsl", "g++", "-o", "a.out"] + programa + ["-std=c++11"]
        wsl_result = subprocess.run(compile_wsl, capture_output=True, text=True)
        if wsl_result.returncode != 0:
            print("❌ WSL g++ también falló:\n", wsl_result.stderr)
            if os.path.exists("a.out"):
                print("⚠️ Usando binario previo 'a.out' (podría estar desactualizado).")
            else:
                print("Instala MinGW-w64 o habilita WSL con g++ para recompilar.")
                exit(1)
        else:
            run_prefix = ["wsl"]
            print("✅ Compilación exitosa (WSL)\n")
    
    # Seleccionar carpeta
    if args.all:
        selected = "ALL"
    elif args.folder:
        selected = args.folder
    else:
        selected = select_folder()

    if selected is None:
        return

    # Ejecutar según selección
    if selected == "ALL":
        subfolders = get_input_subfolders()
        for subfolder in subfolders:
            output_dir = os.path.join("outputs", subfolder)
            run_inputs_from_folder(subfolder, output_dir, run_prefix)
    else:
        output_dir = os.path.join("outputs", selected)
        run_inputs_from_folder(selected, output_dir, run_prefix)
    
    print(f"\n{'='*60}")
    print("✅ PROCESO COMPLETADO")
    print(f"{'='*60}\n")

if __name__ == "__main__":
    main()