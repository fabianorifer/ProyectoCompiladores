import os
import subprocess
import shutil
import sys

def get_input_subfolders():
    
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

def run_inputs_from_folder(subfolder_name, output_dir, exe_path):
    
    input_dir = os.path.join("inputs", subfolder_name)
    os.makedirs(output_dir, exist_ok=True)
    
    
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
        run_cmd = [exe_path, filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)
        
       
        asm_file = os.path.join(input_dir, f"{base_name}.s")
        
        
        if os.path.isfile(asm_file):
            dest_asm = os.path.join(output_dir, f"{base_name}.s")
            shutil.copy2(asm_file, dest_asm)
            os.remove(asm_file)  
            print("✅")
        else:
            print("⚠️  (no se generó .s)")
        
        
        if result.stderr:
            print(f"   ⚠️  Error: {result.stderr.strip()}")

def main():
    
    programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]
    
   
    print("\n" + "="*60)
    print("🔨 COMPILANDO PROYECTO")
    print("="*60)
    # Detectar plataforma para nombre del ejecutable
    is_windows = sys.platform.startswith("win")
    exe_name = "compiler.exe" if is_windows else "compiler"
    exe_path = os.path.join(".", exe_name)

    # Comando de compilación con salida explícita
    compile = ["g++"] + programa + ["-o", exe_name]
    print("Comando:", " ".join(compile))
    result = subprocess.run(compile, capture_output=True, text=True)
    
    if result.returncode != 0:
        print("❌ Error en compilación:\n", result.stderr)
        exit(1)
    
    print("✅ Compilación exitosa\n")
  

    selected = select_folder()
    
    if selected is None:
        return
    
  
    if selected == "ALL":
        subfolders = get_input_subfolders()
        for subfolder in subfolders:
            output_dir = os.path.join("outputs", subfolder)
            run_inputs_from_folder(subfolder, output_dir, exe_path)
    else:
        output_dir = os.path.join("outputs", selected)
        run_inputs_from_folder(selected, output_dir, exe_path)
    
    print(f"\n{'='*60}")
    print("✅ PROCESO COMPLETADO")
    print(f"{'='*60}\n")

if __name__ == "__main__":
    main()