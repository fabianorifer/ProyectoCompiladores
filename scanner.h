#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"
using namespace std;

class Scanner {
private:
    string input;
    int first;
    int current;
    int line;      // tracking de línea
    int column;    // tracking de columna

    // Métodos auxiliares privados
    char nextChar();
    char peekChar(int offset = 0);  // Mirar sin consumir
    void rollBack();
    void startLexema();
    string getLexema();
    
    // Métodos de escaneo específicos
    Token* scanNumber();           // Números (int y float)
    Token* scanIdentifier();       // Identificadores y keywords
    Token* scanString();           // String literals
    Token* scanOperator();         // Operadores compuestos
    
    // Verificadores
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNum(char c);
    bool isWhitespace(char c);
    
    // Manejo de keywords
    Token* checkKeyword(const string& lexema);

public:
    // Constructor
    Scanner(const char* in_s);
    Scanner(const string& in_s);

    // Retorna el siguiente token
    Token* nextToken();

    // Destructor
    ~Scanner();
};

// Ejecutar scanner
int ejecutar_scanner(Scanner* scanner, const string& InputFile);

#endif // SCANNER_H