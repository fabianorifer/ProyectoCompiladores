#include <iostream>
#include <cstring>
#include <fstream>
#include <cctype>
#include "token.h"
#include "scanner.h"

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
Scanner::Scanner(const char* s) 
    : input(s), first(0), current(0), line(1), column(1) { }

Scanner::Scanner(const string& s) 
    : input(s), first(0), current(0), line(1), column(1) { }

// -----------------------------
// Métodos auxiliares privados
// -----------------------------

char Scanner::nextChar() {
    if (current >= input.length()) return '\0';
    char c = input[current];
    current++;
    column++;
    if (c == '\n') {
        line++;
        column = 1;
    }
    return c;
}

char Scanner::peekChar(int offset) {
    int pos = current + offset;
    if (pos >= input.length()) return '\0';
    return input[pos];
}

void Scanner::rollBack() {
    if (current > 0) {
        current--;
        column--;
    }
}

void Scanner::startLexema() {
    first = current;
}

string Scanner::getLexema() {
    return input.substr(first, current - first);
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlphaNum(char c) {
    return isAlpha(c) || isDigit(c);
}

bool Scanner::isWhitespace(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// -----------------------------
// Verificar keywords
// -----------------------------
Token* Scanner::checkKeyword(const string& lexema) {
    // Control de flujo
    if (lexema == "if")     return new Token(Token::IF, lexema, line, column);
    if (lexema == "else")   return new Token(Token::ELSE, lexema, line, column);
    if (lexema == "while")  return new Token(Token::WHILE, lexema, line, column);
    if (lexema == "for")    return new Token(Token::FOR, lexema, line, column);
    if (lexema == "in")     return new Token(Token::IN, lexema, line, column);
    if (lexema == "return") return new Token(Token::RETURN, lexema, line, column);
    
    // Declaraciones
    if (lexema == "let")    return new Token(Token::LET, lexema, line, column);
    if (lexema == "mut")    return new Token(Token::MUT, lexema, line, column);
    if (lexema == "fn")     return new Token(Token::FN, lexema, line, column);
    
    // Tipos
    if (lexema == "i32")    return new Token(Token::I32, lexema, line, column);
    if (lexema == "i64")    return new Token(Token::I64, lexema, line, column);
    if (lexema == "f32")    return new Token(Token::F32, lexema, line, column);
    if (lexema == "f64")    return new Token(Token::F64, lexema, line, column);
    if (lexema == "bool")   return new Token(Token::BOOL, lexema, line, column);
    
    // Literales booleanos
    if (lexema == "true")   return new Token(Token::TRUE, lexema, line, column);
    if (lexema == "false")  return new Token(Token::FALSE, lexema, line, column);
    
    // Conversión
    if (lexema == "as")     return new Token(Token::AS, lexema, line, column);
    if (lexema == "unsafe") return new Token(Token::UNSAFE, lexema, line, column);
    
    // Keywords obsoletas (compatibilidad con código viejo)
    if (lexema == "var")      return new Token(Token::VAR, lexema, line, column);
    if (lexema == "fun")      return new Token(Token::FUN, lexema, line, column);
    if (lexema == "endfun")   return new Token(Token::ENDFUN, lexema, line, column);
    if (lexema == "print")    return new Token(Token::PRINT, lexema, line, column);
    if (lexema == "then")     return new Token(Token::THEN, lexema, line, column);
    if (lexema == "do")       return new Token(Token::DO, lexema, line, column);
    if (lexema == "endif")    return new Token(Token::ENDIF, lexema, line, column);
    if (lexema == "endwhile") return new Token(Token::ENDWHILE, lexema, line, column);
    if (lexema == "sqrt")     return new Token(Token::SQRT, lexema, line, column);
    
    // Macros: println!
    if (lexema == "println!") return new Token(Token::PRINTLN, lexema, line, column);
    
    // Si no es keyword, es un identificador
    return new Token(Token::ID, lexema, line, column);
}

// -----------------------------
// Escanear números (int y float)
// -----------------------------
Token* Scanner::scanNumber() {
    startLexema();
    
    // Parte entera
    while (isDigit(peekChar())) {
        nextChar();
    }
    
    // Verificar si es flotante (tiene punto decimal)
    if (peekChar() == '.' && isDigit(peekChar(1))) {
        nextChar(); // consumir '.'
        
        // Parte decimal
        while (isDigit(peekChar())) {
            nextChar();
        }
        
        
        if (peekChar() == 'e' || peekChar() == 'E') {
            nextChar(); 
            
            // Signo opcional
            if (peekChar() == '+' || peekChar() == '-') {
                nextChar();
            }
            
            // Exponente
            if (!isDigit(peekChar())) {
                return new Token(Token::ERR, getLexema(), line, column);
            }
            
            while (isDigit(peekChar())) {
                nextChar();
            }
        }
        
        return new Token(Token::FLOAT, getLexema(), line, column);
    }
    
    // Es un entero
    return new Token(Token::NUM, getLexema(), line, column);
}

// -----------------------------
// Escanear identificadores y keywords
// -----------------------------
Token* Scanner::scanIdentifier() {
    startLexema();
    
    // Primer carácter ya fue verificado (letra o _)
    while (isAlphaNum(peekChar())) {
        nextChar();
    }
    
    // Verificar si termina con '!' (macro como println!)
    if (peekChar() == '!') {
        nextChar();
    }
    
    string lexema = getLexema();
    return checkKeyword(lexema);
}

// -----------------------------
// Escanear strings
// -----------------------------
Token* Scanner::scanString() {
    startLexema();
    nextChar(); // consumir '"' inicial
    
    string value = "";
    
    while (peekChar() != '"' && peekChar() != '\0') {
        if (peekChar() == '\\') {
            // Escape sequence
            nextChar(); // consumir '\'
            char escaped = nextChar();
            
            switch (escaped) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case 'r':  value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"':  value += '"';  break;
                case '0':  value += '\0'; break;
                default:   value += escaped; break;
            }
        } else {
            value += nextChar();
        }
    }
    
    if (peekChar() != '"') {
        // String no cerrado
        return new Token(Token::ERR, "Unterminated string", line, column);
    }
    
    nextChar(); // consumir '"' final
    
    return new Token(Token::STRING, value, line, column);
}

// -----------------------------
// nextToken: obtiene el siguiente token
// -----------------------------
Token* Scanner::nextToken() {
    // Saltar espacios en blanco
    while (current < input.length() && isWhitespace(peekChar())) {
        nextChar();
    }
    
    // Saltar comentarios de línea //
    if (peekChar() == '/' && peekChar(1) == '/') {
        // Consumir hasta fin de línea
        while (peekChar() != '\n' && peekChar() != '\0') {
            nextChar();
        }
        // Recursivamente obtener el siguiente token
        return nextToken();
    }
    
    // Fin de la entrada
    if (current >= input.length()) {
        return new Token(Token::END);
    }
    
    char c = peekChar();
    
    // Números
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // Identificadores y keywords
    if (isAlpha(c)) {
        return scanIdentifier();
    }
    
    // Strings
    if (c == '"') {
        return scanString();
    }
    
    // Operadores y delimitadores
    startLexema();
    nextChar(); // consumir primer carácter
    
    Token* token = nullptr;
    
    switch (c) {
        // Operadores compuestos que necesitan lookahead
        case '+':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::PLUS_ASSIGN, "+=", line, column);
            } else {
                token = new Token(Token::PLUS, "+", line, column);
            }
            break;
            
        case '-':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::MINUS_ASSIGN, "-=", line, column);
            } else if (peekChar() == '>') {
                nextChar();
                token = new Token(Token::ARROW, "->", line, column);
            } else {
                token = new Token(Token::MINUS, "-", line, column);
            }
            break;
            
        case '*':
            if (peekChar() == 'm' && peekChar(1) == 'u' && peekChar(2) == 't') {
                // *mut
                nextChar(); nextChar(); nextChar();
                token = new Token(Token::STAR_MUT, "*mut", line, column);
            } else if (peekChar() == 'c' && peekChar(1) == 'o' && 
                       peekChar(2) == 'n' && peekChar(3) == 's' && peekChar(4) == 't') {
                // *const
                nextChar(); nextChar(); nextChar(); nextChar(); nextChar();
                token = new Token(Token::STAR_CONST, "*const", line, column);
            } else if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::MUL_ASSIGN, "*=", line, column);
            } else if (peekChar() == '*') {
                nextChar();
                token = new Token(Token::POW, "**", line, column);
            } else {
                token = new Token(Token::MUL, "*", line, column);
            }
            break;
            
        case '/':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::DIV_ASSIGN, "/=", line, column);
            } else {
                token = new Token(Token::DIV, "/", line, column);
            }
            break;
            
        case '%':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::MOD_ASSIGN, "%=", line, column);
            } else {
                token = new Token(Token::MOD, "%", line, column);
            }
            break;
            
        case '=':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::EQ, "==", line, column);
            } else {
                token = new Token(Token::ASSIGN, "=", line, column);
            }
            break;
            
        case '!':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::NE, "!=", line, column);
            } else {
                token = new Token(Token::NOT, "!", line, column);
            }
            break;
            
        case '<':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::LE, "<=", line, column);
            } else {
                token = new Token(Token::LT, "<", line, column);
            }
            break;
            
        case '>':
            if (peekChar() == '=') {
                nextChar();
                token = new Token(Token::GE, ">=", line, column);
            } else {
                token = new Token(Token::GT, ">", line, column);
            }
            break;
            
        case '&':
            if (peekChar() == '&') {
                nextChar();
                token = new Token(Token::AND, "&&", line, column);
            } else {
                token = new Token(Token::AMP, "&", line, column);
            }
            break;
            
        case '|':
            if (peekChar() == '|') {
                nextChar();
                token = new Token(Token::OR, "||", line, column);
            } else {
                
                token = new Token(Token::ERR, "|", line, column);
            }
            break;
            
        case '.':
            if (peekChar() == '.') {
                nextChar();
                token = new Token(Token::DOUBLE_DOT, "..", line, column);
            } else {
               
                token = new Token(Token::ERR, ".", line, column);
            }
            break;
            
        // Delimitadores simples
        case '(': token = new Token(Token::LPAREN, "(", line, column); break;
        case ')': token = new Token(Token::RPAREN, ")", line, column); break;
        case '{': token = new Token(Token::LBRACE, "{", line, column); break;
        case '}': token = new Token(Token::RBRACE, "}", line, column); break;
        case ';': token = new Token(Token::SEMICOL, ";", line, column); break;
        case ',': token = new Token(Token::COMA, ",", line, column); break;
        case ':': token = new Token(Token::COLON, ":", line, column); break;
        
        // Carácter inválido
        default:
            token = new Token(Token::ERR, string(1, c), line, column);
            break;
    }
    
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
            return 1;
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