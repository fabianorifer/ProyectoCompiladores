#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "token.h"

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    string content, line;
    while (getline(infile, line)) {
        content += line + '\n';
    }
    infile.close();

    Scanner scanner(content.c_str());
    while (true) {
        Token* tok = scanner.nextToken();
        if (!tok) {
            cerr << "Scanner devolvió NULL" << endl;
            break;
        }
        cout << tok << endl; // usa operator<< con posición
        if (tok->type == Token::END) {
            delete tok;
            break;
        }
        delete tok; // liberar cada token
    }

    return 0;
}
