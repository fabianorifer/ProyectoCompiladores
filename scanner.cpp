#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
Scanner::Scanner(const char* s): input(s), first(0), current(0) { 
    }

// -----------------------------
// Función auxiliar
// -----------------------------

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// -----------------------------
// nextToken: obtiene el siguiente token
// -----------------------------


Token* Scanner::nextToken() {
    Token* token;

    // Saltar espacios en blanco y comentarios
    while (current < input.length()) {
        if (is_white_space(input[current])) {
            current++;
            continue;
        }
        // Comentarios de línea //
        if (current + 1 < input.length() && input[current] == '/' && input[current + 1] == '/') {
            current += 2;
            while (current < input.length() && input[current] != '\n') {
                current++;
            }
            continue;
        }
        break;
    }

    // Fin de la entrada
    if (current >= input.length()) 
        return new Token(Token::END);

    char c = input[current];
    first = current;

    // Números (enteros y flotantes)
    if (isdigit(c)) {
        current++;
        bool is_float = false;
        
        // Parte entera
        while (current < input.length() && isdigit(input[current]))
            current++;
        
        // Punto decimal
        if (current < input.length() && input[current] == '.') {
            // Verificar que no sea .. (rango)
            if (current + 1 < input.length() && input[current + 1] == '.') {
                // Es un rango, no un float
                return new Token(Token::NUM, input, first, current - first);
            }
            // Es un float
            is_float = true;
            current++;
            while (current < input.length() && isdigit(input[current]))
                current++;
            
            // Notación científica (e o E)
            if (current < input.length() && (input[current] == 'e' || input[current] == 'E')) {
                current++;
                if (current < input.length() && (input[current] == '+' || input[current] == '-'))
                    current++;
                while (current < input.length() && isdigit(input[current]))
                    current++;
            }
        }
        
        return new Token(is_float ? Token::FLOAT : Token::NUM, input, first, current - first);
    }
    
    // String literals
    if (c == '"') {
        current++;
        while (current < input.length() && input[current] != '"') {
            if (input[current] == '\\' && current + 1 < input.length()) {
                current += 2; // Saltar caracteres escapados
            } else {
                current++;
            }
        }
        if (current < input.length()) {
            current++; // Incluir la comilla de cierre
        }
        return new Token(Token::STRING, input, first, current - first);
    }
    
    // Identificadores y keywords
    if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_'))
            current++;
        
        // Verificar si es println!
        if (current < input.length() && input[current] == '!' && 
            input.substr(first, current - first) == "println") {
            current++; // incluir el !
            return new Token(Token::PRINTLN, input, first, current - first);
        }
        
        string lexema = input.substr(first, current - first);
        
        // Keywords
        if (lexema == "let") return new Token(Token::LET, input, first, current - first);
        if (lexema == "mut") return new Token(Token::MUT, input, first, current - first);
        if (lexema == "fn") return new Token(Token::FN, input, first, current - first);
        if (lexema == "return") return new Token(Token::RETURN, input, first, current - first);
        if (lexema == "if") return new Token(Token::IF, input, first, current - first);
        if (lexema == "else") return new Token(Token::ELSE, input, first, current - first);
        if (lexema == "while") return new Token(Token::WHILE, input, first, current - first);
        if (lexema == "for") return new Token(Token::FOR, input, first, current - first);
        if (lexema == "in") return new Token(Token::IN, input, first, current - first);
        if (lexema == "as") return new Token(Token::AS, input, first, current - first);
        
        // Tipos
        if (lexema == "i32") return new Token(Token::I32, input, first, current - first);
        if (lexema == "i64") return new Token(Token::I64, input, first, current - first);
        if (lexema == "f32") return new Token(Token::F32, input, first, current - first);
        if (lexema == "f64") return new Token(Token::F64, input, first, current - first);
        if (lexema == "bool") return new Token(Token::BOOL, input, first, current - first);
        
        // Booleanos
        if (lexema == "true") return new Token(Token::TRUE, input, first, current - first);
        if (lexema == "false") return new Token(Token::FALSE, input, first, current - first);
        
        // Identificador
        return new Token(Token::ID, input, first, current - first);
    }
    
    // Operadores de dos caracteres
    if (current + 1 < input.length()) {
        string two_char = input.substr(current, 2);
        
        if (two_char == "==") { current += 2; return new Token(Token::EQ, input, first, 2); }
        if (two_char == "!=") { current += 2; return new Token(Token::NEQ, input, first, 2); }
        if (two_char == "<=") { current += 2; return new Token(Token::LE, input, first, 2); }
        if (two_char == ">=") { current += 2; return new Token(Token::GE, input, first, 2); }
        if (two_char == "&&") { current += 2; return new Token(Token::AND, input, first, 2); }
        if (two_char == "||") { current += 2; return new Token(Token::OR, input, first, 2); }
        if (two_char == "+=") { current += 2; return new Token(Token::PLUS_ASSIGN, input, first, 2); }
        if (two_char == "-=") { current += 2; return new Token(Token::MINUS_ASSIGN, input, first, 2); }
        if (two_char == "*=") { current += 2; return new Token(Token::MUL_ASSIGN, input, first, 2); }
        if (two_char == "/=") { current += 2; return new Token(Token::DIV_ASSIGN, input, first, 2); }
        if (two_char == "%=") { current += 2; return new Token(Token::MOD_ASSIGN, input, first, 2); }
        if (two_char == "**") { current += 2; return new Token(Token::POW, input, first, 2); }
        if (two_char == "->") { current += 2; return new Token(Token::ARROW, input, first, 2); }
        if (two_char == "..") { current += 2; return new Token(Token::DOTDOT, input, first, 2); }
    }
    
    // Punteros especiales: *mut, *const, &mut
    if (c == '*') {
        // Diferenciar entre operador de desreferencia y prefijo de tipo puntero (*mut / *const)
        int saved = current;
        current++;
        // Mirar siguiente palabra sin consumir definitivamente
        int look = current;
        while (look < input.length() && is_white_space(input[look])) look++;

        auto isBoundary = [&](int start, int len) -> bool {
            int pos = start + len;
            if (pos >= input.length()) return true;
            char after = input[pos];
            // Debe terminar en espacio, separador o símbolo de tipo (por simplicidad: espacio o i/f/b)
            return is_white_space(after);
        };

        if (look + 2 < input.length()) {
            string next3 = input.substr(look, 3);
            if (next3 == "mut" && isBoundary(look, 3)) {
                current = look + 3;
                return new Token(Token::PTR_MUT, input, first, current - first);
            }
        }
        if (look + 4 < input.length()) {
            string next5 = input.substr(look, 5);
            if (next5 == "const" && isBoundary(look, 5)) {
                current = look + 5;
                return new Token(Token::PTR_CONST, input, first, current - first);
            }
        }
        // Si seguido viene letra / '_' que forma identificador => es desreferencia antes de variable
        current = saved + 1; // solo consume '*'
        return new Token(Token::MUL, '*');
    }
    
    if (c == '&') {
        // Verificar &mut
        int saved = current;
        current++;
        while (current < input.length() && is_white_space(input[current]))
            current++;
        
        if (current + 2 < input.length()) {
            string next = input.substr(current, 3);
            if (next == "mut") {
                current += 3;
                return new Token(Token::AMP_MUT, input, first, current - first);
            }
        }
        
        // Solo &
        current = saved + 1;
        return new Token(Token::AMP, c);
    }
    
    // Operadores de un carácter
    switch (c) {
        case '+': token = new Token(Token::PLUS, c); break;
        case '-': token = new Token(Token::MINUS, c); break;
        case '/': token = new Token(Token::DIV, c); break;
        case '%': token = new Token(Token::MOD, c); break;
        case '(': token = new Token(Token::LPAREN, c); break;
        case ')': token = new Token(Token::RPAREN, c); break;
        case '{': token = new Token(Token::LBRACE, c); break;
        case '}': token = new Token(Token::RBRACE, c); break;
        case ';': token = new Token(Token::SEMICOL, c); break;
        case ',': token = new Token(Token::COMA, c); break;
        case ':': token = new Token(Token::COLON, c); break;
        case '<': token = new Token(Token::LT, c); break;
        case '>': token = new Token(Token::GT, c); break;
        case '=': token = new Token(Token::ASSIGN, c); break;
        case '!': token = new Token(Token::NOT, c); break;
        default:
            token = new Token(Token::ERR, c);
    }
    current++;
    
    return token;
}




// -----------------------------
// Destructor
// -----------------------------
Scanner::~Scanner() { }

// -----------------------------
// Función de prueba
// -----------------------------

int ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    // Crear nombre para archivo de salida
    string OutputFileName = InputFile;
    size_t pos = OutputFileName.find_last_of(".");
    if (pos != string::npos) {
        OutputFileName = OutputFileName.substr(0, pos);
    }
    OutputFileName += "_tokens.txt";

    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
        return 0;
    }

    outFile << "Scanner\n" << endl;

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << endl;
            delete tok;
            outFile << "\nScanner exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido" << endl << endl;
            outFile << "Scanner no exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        outFile << *tok << endl;
        delete tok;
    }
}
