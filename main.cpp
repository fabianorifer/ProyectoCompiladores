#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"

// Touch file to trigger recompile after AST guard change

using namespace std;

int main(int argc, const char* argv[]) {
    // Permitir modo debug vía variable de entorno COMP_DEBUG
    bool debugMode = (getenv("COMP_DEBUG") != nullptr);
    bool debugTokens = (getenv("COMP_DEBUG_TOKENS") != nullptr);
    // Verificar número de argumentos (permitimos exactamente 1 archivo)
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        cout << "(Opcional: export COMP_DEBUG=1 para logs de parser)" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner 
    Scanner scanner1(input.c_str());
    if (debugTokens) {
        cerr << "[TOKDUMP] Inicio volcado de tokens" << endl;
        Scanner temp(input.c_str());
        Token* tk;
        int idx=0;
        while ((tk = temp.nextToken())->type != Token::END) {
            cerr << "[TOK] #" << idx++ << " type=" << tk->type << " text=" << tk->text << endl;
            if (tk->type == Token::ERR) { cerr << "[TOK] Error léxico" << endl; delete tk; break; }
            delete tk;
        }
        cerr << "[TOKDUMP] Fin volcado" << endl;
    }

    // Crear instancias de Parser
    Parser parser(&scanner1);
    if (debugMode) {
        cerr << "[DEBUG] Parser en modo depuración" << endl;
        parser.setDebug(true);
    }

    // Parsear y generar AST
  
    Program* program = parser.parseProgram();     
        string inputFile(argv[1]);
        size_t dotPos = inputFile.find_last_of('.');
        string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
        string outputFilename = baseName + ".s";
        ofstream outfile(outputFilename);
        if (!outfile.is_open()) {
            cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
            return 1;
        }

    cout << "Generando codigo ensamblador en " << outputFilename << endl;
    GenCodeVisitor codigo(outfile);
    codigo.generar(program);
    outfile.close();
    
    return 0;
}
