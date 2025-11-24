#!/usr/bin/env python3
import os, subprocess, json, shutil, sys, tempfile
from pathlib import Path

DESCRIPTION = """Valida programas ensamblador (.s) generados en outputs/.

Flujo:
 1. Recorre todas las subcarpetas de outputs/ buscando archivos .s
 2. Ensambla cada uno con nasm (-felf64) y luego enlaza con gcc (usa printf)
 3. Ejecuta el binario y captura stdout (stderr ignorado salvo error)
 4. Compara la salida con los valores esperados (baseline) si existe un archivo de expectativas

Modos:
  --expected expected_outputs.json   Archivo JSON con mapping {"relative/path/to/file.s": "salida esperada"}
  --generate-baseline                Genera/actualiza baseline a partir de salida actual (no marca fallos)
  --fail-on-missing                  Si falta entrada en baseline y no está en modo generación, marcar error
  --only-folder FOLDER              Limita validación a outputs/FOLDER

Formato JSON baseline:
  {
    "BaseInputs5/input1.s": "Factorial: 120\n",
    ...
  }

Nota: Si aún no tienes baseline, primero ejecuta con --generate-baseline y revisa el archivo.

Requisitos: nasm y gcc en PATH (ideal ejecutar dentro de WSL). En Windows puro puedes anteponer 'wsl ' manualmente.
"""

def find_asm_files(root: Path, only_folder: str|None):
    asm_files = []
    for sub in root.iterdir():
        if not sub.is_dir():
            continue
        if only_folder and sub.name != only_folder:
            continue
        for f in sub.glob('*.s'):
            asm_files.append(f)
    return asm_files

def assemble_and_link(asm_path: Path, workdir: Path):
    from shutil import which
    obj_path = workdir / (asm_path.stem + '.o')
    exe_path = workdir / (asm_path.stem)
    use_wsl = False
    if which('nasm') is None:
        # Intentar WSL fallback
        if which('wsl') is None:
            return False, "nasm no encontrado. Instala con: sudo apt update && sudo apt install nasm"
        use_wsl = True

    def win_to_wsl(p: str) -> str:
        # C:\Users\user -> /mnt/c/Users/user
        if len(p) > 2 and p[1] == ':':
            return '/mnt/' + p[0].lower() + p[2:].replace('\\', '/')
        return p.replace('\\', '/')

    if use_wsl:
        asm_p = win_to_wsl(str(asm_path.resolve()))
        obj_p = win_to_wsl(str(obj_path.resolve()))
        exe_p = win_to_wsl(str(exe_path.resolve()))
        nasm_cmd = ['wsl', 'nasm', '-felf64', asm_p, '-o', obj_p]
        gcc_cmd = ['wsl', 'gcc', obj_p, '-o', exe_p, '-no-pie', '-z', 'noexecstack']
        run_cmd = ['wsl', exe_p]
    else:
        nasm_cmd = ['nasm', '-felf64', str(asm_path), '-o', str(obj_path)]
        gcc_cmd = ['gcc', str(obj_path), '-o', str(exe_path), '-no-pie', '-z', 'noexecstack']
        run_cmd = [str(exe_path)]

    r = subprocess.run(nasm_cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return False, f"NASM fallo: {r.stderr.strip()}"
    r = subprocess.run(gcc_cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return False, f"GCC fallo: {r.stderr.strip()}"
    r = subprocess.run(run_cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return False, f"Ejecucion fallo rc={r.returncode} stderr={r.stderr.strip()}"
    return True, r.stdout

def load_expected(path: Path):
    if not path.exists():
        return {}
    with path.open('r', encoding='utf-8') as f:
        return json.load(f)

def save_expected(path: Path, data: dict):
    with path.open('w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)

def normalize_output(s: str):
    # Quitar espacios finales y normalizar fin de linea
    return '\n'.join(line.rstrip() for line in s.splitlines()) + ('\n' if s and not s.endswith('\n') else '')

def main():
    import argparse
    ap = argparse.ArgumentParser(description='Valida salidas de .s generados', epilog='Usa --generate-baseline para crear baseline inicial.')
    ap.add_argument('--expected', default='expected_outputs.json', help='Archivo JSON de baseline esperada')
    ap.add_argument('--generate-baseline', action='store_true', help='Genera/actualiza baseline en lugar de validar')
    ap.add_argument('--fail-on-missing', action='store_true', help='Fallar si falta salida esperada')
    ap.add_argument('--only-folder', help='Solo validar una subcarpeta de outputs/')
    ap.add_argument('--print-output', action='store_true', help='Imprimir siempre la salida real')
    ap.add_argument('--show-diff', action='store_true', help='Mostrar diff cuando difiere')
    ap.add_argument('--keep-temp', action='store_true', help='No borrar temporales (obj/exec)')
    args = ap.parse_args()

    root = Path('outputs')
    if not root.exists():
        print('❌ No existe carpeta outputs/')
        sys.exit(1)

    asm_files = find_asm_files(root, args.only_folder)
    if not asm_files:
        print('⚠️  No hay archivos .s para validar')
        sys.exit(0)

    expected_map = load_expected(Path(args.expected))
    updated_map = dict(expected_map)  # para baseline nueva

    total = len(asm_files)
    ok = 0
    fail = 0

    with tempfile.TemporaryDirectory() as tmpdir_str:
        tmpdir = Path(tmpdir_str)
        for asm in asm_files:
            rel_key = f"{asm.parent.name}/{asm.name}"
            print(f"▶ {rel_key}")
            success, result = assemble_and_link(asm, tmpdir)
            if not success:
                print(f"  ❌ Compilación/Ejecución falló: {result}")
                fail += 1
                continue
            real_out_norm = normalize_output(result)
            if args.generate_baseline:
                updated_map[rel_key] = real_out_norm
                print("  ✅ Capturado para baseline")
                ok += 1
            else:
                expected = expected_map.get(rel_key)
                if expected is None:
                    msg = 'Sin baseline' if not args.fail_on_missing else '❌ Falta baseline'
                    print(f"  ⚠️ {msg}")
                    if args.fail_on_missing:
                        fail += 1
                    else:
                        ok += 1  # no se considera fallo
                    if args.print_output:
                        print("  ⬇ Salida real:\n" + real_out_norm)
                else:
                    if real_out_norm == expected:
                        print("  ✅ Coincide")
                        ok += 1
                        if args.print_output:
                            print("  ⬇ Salida real:\n" + real_out_norm)
                    else:
                        print("  ❌ Diferente")
                        fail += 1
                        if args.print_output or True:
                            print("  ⬇ Real:\n" + real_out_norm)
                            print("  ⬇ Esperado:\n" + expected)
                        if args.show_diff:
                            import difflib
                            diff = difflib.unified_diff(expected.splitlines(), real_out_norm.splitlines(), fromfile='expected', tofile='real', lineterm='')
                            for line in diff:
                                print('    ' + line)
        if args.keep_temp:
            print(f"(Info) Temporales en {tmpdir}")
        else:
            # Directorio temporal se borra automaticamente
            pass

    if args.generate_baseline:
        save_expected(Path(args.expected), updated_map)
        print(f"\n💾 Baseline actualizada en {args.expected}")
    print("\n==============================")
    print("Resumen:")
    print(f"  Archivos: {total}")
    print(f"  OK: {ok}")
    print(f"  Fallos: {fail}")
    print("==============================")
    if fail > 0 and not args.generate_baseline:
        sys.exit(2)

if __name__ == '__main__':
    # Mostrar descripción si se invoca sin args adicionales
    if len(sys.argv) == 1:
        print(DESCRIPTION)
    main()
