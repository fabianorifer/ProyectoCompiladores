import os
import subprocess
import shutil

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
    """Ejecuta todos los inputs de una subcarpeta específica"""
    input_dir = os.path.join("inputs", subfolder_name)
    os.makedirs(output_dir, exist_ok=True)
    
    # Obtener todos los archivos .txt
    input_files = sorted([f for f in os.listdir(input_dir) if f.endswith('.txt')])
    
    if not input_files:
        print(f"❌ No se encontraron archivos .txt en '{input_dir}'")
        return
    
    print(f"\n{'='*60}")
    print(f"🚀 Ejecutando {len(input_files)} inputs de: {subfolder_name}")
    print(f"{'='*60}\n")
    
    for input_file in input_files:
        filepath = os.path.join(input_dir, input_file)
        base_name = os.path.splitext(input_file)[0]
        
        print(f"▶️  Ejecutando {input_file}...", end=" ")
        run_cmd = ["./a.out", filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)
        
        # Archivo .s generado (se crea en la misma carpeta del input)
        asm_file = os.path.join(input_dir, f"{base_name}.s")
        
        # Mover archivo si existe
        if os.path.isfile(asm_file):
            dest_asm = os.path.join(output_dir, f"{base_name}.s")
            shutil.move(asm_file, dest_asm)
            print("✅")
        else:
            print("⚠️  (no se generó .s)")
        
        # Mostrar errores si los hay
        if result.stderr:
            print(f"   ⚠️  Error: {result.stderr.strip()}")

def main():
    # Archivos c++
    programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]
    
    # Compilar
    print("\n" + "="*60)
    print("🔨 COMPILANDO PROYECTO")
    print("="*60)
    compile = ["g++"] + programa
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