import os
import subprocess
import shutil
import glob

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

def run_inputs_from_folder(subfolder_name, output_dir):
    """Ejecuta todos los inputs de una subcarpeta específica y guarda los tokens"""
    input_dir = os.path.join("inputs", subfolder_name)
    scanner_output_dir = os.path.join(output_dir, "scanner_outputs")
    os.makedirs(scanner_output_dir, exist_ok=True)
    
    # Obtener todos los archivos .txt
    input_files = sorted([f for f in os.listdir(input_dir) if f.endswith('.txt')])
    
    if not input_files:
        print(f"❌ No se encontraron archivos .txt en '{input_dir}'")
        return
    
    print(f"\n{'='*60}")
    print(f"🚀 Ejecutando {len(input_files)} inputs de: {subfolder_name}")
    print(f"{'='*60}\n")
    
    success_count = 0
    fail_count = 0
    
    for input_file in input_files:
        filepath = os.path.join(input_dir, input_file)
        base_name = os.path.splitext(input_file)[0]
        
        print(f"▶️  Ejecutando {input_file}...", end=" ")
        run_cmd = ["./a.out", filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)
        
        # Archivo _tokens.txt generado por el scanner
        tokens_file = os.path.join(input_dir, f"{base_name}_tokens.txt")
        
        # Verificar si se generó exitosamente
        scanner_success = False
        if os.path.isfile(tokens_file):
            # Leer el archivo para verificar si fue exitoso
            with open(tokens_file, 'r', encoding='utf-8') as f:
                content = f.read()
                if "Scanner exitoso" in content:
                    scanner_success = True
                    success_count += 1
                else:
                    fail_count += 1
            
            # Mover archivo de tokens a la carpeta de output
            dest_tokens = os.path.join(scanner_output_dir, f"{base_name}_tokens.txt")
            shutil.move(tokens_file, dest_tokens)
            
            if scanner_success:
                print("✅ Scanner exitoso")
            else:
                print("❌ Scanner no exitoso")
        else:
            print("⚠️  (no se generó archivo de tokens)")
            fail_count += 1
        
        # Archivo .s generado (si existe)
        asm_file = os.path.join(input_dir, f"{base_name}.s")
        if os.path.isfile(asm_file):
            dest_asm = os.path.join(output_dir, f"{base_name}.s")
            shutil.move(asm_file, dest_asm)
        
        # Mostrar errores si los hay
        if result.stderr:
            print(f"   ⚠️  Error: {result.stderr.strip()}")
    
    # Reporte final
    print(f"\n{'='*60}")
    print(f"📊 RESUMEN - {subfolder_name}")
    print(f"{'='*60}")
    print(f"   ✅ Scanner exitoso: {success_count}/{len(input_files)}")
    print(f"   ❌ Scanner no exitoso: {fail_count}/{len(input_files)}")
    print(f"   📁 Tokens guardados en: {scanner_output_dir}")
    print(f"{'='*60}\n")

def main():
    # Archivos c++ para el scanner
    # Usamos test_scanner.cpp en lugar de main.cpp para probar solo el scanner
    # y evitar errores de compilación del parser que aún no ha sido actualizado
    programa = ["test_scanner.cpp", "scanner.cpp", "token.cpp"]
    
    # Compilar
    print("\n" + "="*60)
    print("🔨 COMPILANDO SCANNER")
    print("="*60)
    compile = ["g++", "-o", "a.out"] + programa + ["-std=c++11"]
    print("Comando:", " ".join(compile))
    result = subprocess.run(compile, capture_output=True, text=True)
    
    if result.returncode != 0:
        print("❌ Error en compilación:\n", result.stderr)
        exit(1)
    
    print("✅ Compilación exitosa\n")
    
    # Seleccionar carpeta
    selected = select_folder()
    
    if selected is None:
        return
    
    # Ejecutar según selección
    if selected == "ALL":
        subfolders = get_input_subfolders()
        for subfolder in subfolders:
            output_dir = os.path.join("outputs", subfolder)
            run_inputs_from_folder(subfolder, output_dir)
    else:
        output_dir = os.path.join("outputs", selected)
        run_inputs_from_folder(selected, output_dir)
    
    print(f"\n{'='*60}")
    print("✅ PROCESO COMPLETADO")
    print(f"{'='*60}\n")

if __name__ == "__main__":
    main()