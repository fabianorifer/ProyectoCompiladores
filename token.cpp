#include <iostream>
#include "token.h"

using namespace std;

// -----------------------------
// Constructores
// -----------------------------

Token::Token(Type type) 
    : type(type), text(""), line(0), column(0) { }

Token::Token(Type type, char c) 
    : type(type), text(string(1, c)), line(0), column(0) { }

Token::Token(Type type, const string& txt, int lineNumber, int columnNumber)
    : type(type), text(txt), line(lineNumber), column(columnNumber) { }

// -----------------------------
// Método auxiliar typeToString
// -----------------------------

string Token::typeToString(Type type) {
    switch(type) {
        // Operadores aritméticos
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case MOD: return "MOD";
        case POW: return "POW";
        
        // Operadores de asignación
        case ASSIGN: return "ASSIGN";
        case PLUS_ASSIGN: return "PLUS_ASSIGN";
        case MINUS_ASSIGN: return "MINUS_ASSIGN";
        case MUL_ASSIGN: return "MUL_ASSIGN";
        case DIV_ASSIGN: return "DIV_ASSIGN";
        case MOD_ASSIGN: return "MOD_ASSIGN";
        
        // Operadores relacionales
        case LT: return "LT";
        case GT: return "GT";
        case LE: return "LE";
        case GE: return "GE";
        case EQ: return "EQ";
        case NE: return "NE";
        
        // Operadores lógicos
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        
        // Delimitadores
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case SEMICOL: return "SEMICOL";
        case COMA: return "COMA";
        case COLON: return "COLON";
        case ARROW: return "ARROW";
        case DOUBLE_DOT: return "DOUBLE_DOT";
        
        // Referencias/punteros
        case AMP: return "AMP";
        case STAR_MUT: return "STAR_MUT";
        case STAR_CONST: return "STAR_CONST";
        
        // Palabras clave
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case FOR: return "FOR";
        case IN: return "IN";
        case RETURN: return "RETURN";
        case LET: return "LET";
        case MUT: return "MUT";
        case FN: return "FN";
        
        // Tipos
        case I32: return "I32";
        case I64: return "I64";
        case F32: return "F32";
        case F64: return "F64";
        case BOOL: return "BOOL";
        
        // Literales booleanos
        case TRUE: return "TRUE";
        case FALSE: return "FALSE";
        
        // Conversión
        case AS: return "AS";
        case UNSAFE: return "UNSAFE";
        
        // Macros
        case PRINTLN: return "PRINTLN";
        
        // Literales
        case NUM: return "NUM";
        case FLOAT: return "FLOAT";
        case STRING: return "STRING";
        case ID: return "ID";
        
        // Control
        case ERR: return "ERR";
        case END: return "END";
        
        default: return "UNKNOWN";
    }
}

// -----------------------------
// Sobrecarga de operador 
// -----------------------------

// Para Token por referencia
ostream& operator<<(ostream& outs, const Token& tok) {
    switch (tok.type) {
        // Operadores aritméticos
        case Token::PLUS:   outs << "TOKEN(PLUS, \""   << tok.text << "\")"; break;
        case Token::MINUS:  outs << "TOKEN(MINUS, \""  << tok.text << "\")"; break;
        case Token::MUL:    outs << "TOKEN(MUL, \""    << tok.text << "\")"; break;
        case Token::DIV:    outs << "TOKEN(DIV, \""    << tok.text << "\")"; break;
        case Token::MOD:    outs << "TOKEN(MOD, \""    << tok.text << "\")"; break;
        case Token::POW:    outs << "TOKEN(POW, \""    << tok.text << "\")"; break;
        
        // Operadores de asignación
        case Token::ASSIGN:       outs << "TOKEN(ASSIGN, \""       << tok.text << "\")"; break;
        case Token::PLUS_ASSIGN:  outs << "TOKEN(PLUS_ASSIGN, \""  << tok.text << "\")"; break;
        case Token::MINUS_ASSIGN: outs << "TOKEN(MINUS_ASSIGN, \"" << tok.text << "\")"; break;
        case Token::MUL_ASSIGN:   outs << "TOKEN(MUL_ASSIGN, \""   << tok.text << "\")"; break;
        case Token::DIV_ASSIGN:   outs << "TOKEN(DIV_ASSIGN, \""   << tok.text << "\")"; break;
        case Token::MOD_ASSIGN:   outs << "TOKEN(MOD_ASSIGN, \""   << tok.text << "\")"; break;
        
        // Operadores relacionales
        case Token::LT: outs << "TOKEN(LT, \"" << tok.text << "\")"; break;
        case Token::GT: outs << "TOKEN(GT, \"" << tok.text << "\")"; break;
        case Token::LE: outs << "TOKEN(LE, \"" << tok.text << "\")"; break;
        case Token::GE: outs << "TOKEN(GE, \"" << tok.text << "\")"; break;
        case Token::EQ: outs << "TOKEN(EQ, \"" << tok.text << "\")"; break;
        case Token::NE: outs << "TOKEN(NE, \"" << tok.text << "\")"; break;
        
        // Operadores lógicos
        case Token::AND: outs << "TOKEN(AND, \"" << tok.text << "\")"; break;
        case Token::OR:  outs << "TOKEN(OR, \""  << tok.text << "\")"; break;
        case Token::NOT: outs << "TOKEN(NOT, \"" << tok.text << "\")"; break;
        
        // Delimitadores
        case Token::LPAREN:     outs << "TOKEN(LPAREN, \""     << tok.text << "\")"; break;
        case Token::RPAREN:     outs << "TOKEN(RPAREN, \""     << tok.text << "\")"; break;
        case Token::LBRACE:     outs << "TOKEN(LBRACE, \""     << tok.text << "\")"; break;
        case Token::RBRACE:     outs << "TOKEN(RBRACE, \""     << tok.text << "\")"; break;
        case Token::SEMICOL:    outs << "TOKEN(SEMICOL, \""    << tok.text << "\")"; break;
        case Token::COMA:       outs << "TOKEN(COMA, \""       << tok.text << "\")"; break;
        case Token::COLON:      outs << "TOKEN(COLON, \""      << tok.text << "\")"; break;
        case Token::ARROW:      outs << "TOKEN(ARROW, \""      << tok.text << "\")"; break;
        case Token::DOUBLE_DOT: outs << "TOKEN(DOUBLE_DOT, \"" << tok.text << "\")"; break;
        
        // Referencias/punteros
        case Token::AMP:        outs << "TOKEN(AMP, \""        << tok.text << "\")"; break;
        case Token::STAR_MUT:   outs << "TOKEN(STAR_MUT, \""   << tok.text << "\")"; break;
        case Token::STAR_CONST: outs << "TOKEN(STAR_CONST, \"" << tok.text << "\")"; break;
        
        // Palabras clave - control
        case Token::IF:     outs << "TOKEN(IF, \""     << tok.text << "\")"; break;
        case Token::ELSE:   outs << "TOKEN(ELSE, \""   << tok.text << "\")"; break;
        case Token::WHILE:  outs << "TOKEN(WHILE, \""  << tok.text << "\")"; break;
        case Token::FOR:    outs << "TOKEN(FOR, \""    << tok.text << "\")"; break;
        case Token::IN:     outs << "TOKEN(IN, \""     << tok.text << "\")"; break;
        case Token::RETURN: outs << "TOKEN(RETURN, \"" << tok.text << "\")"; break;
        
        // Palabras clave - declaraciones
        case Token::LET: outs << "TOKEN(LET, \"" << tok.text << "\")"; break;
        case Token::MUT: outs << "TOKEN(MUT, \"" << tok.text << "\")"; break;
        case Token::FN:  outs << "TOKEN(FN, \""  << tok.text << "\")"; break;
        
        // Tipos
        case Token::I32:  outs << "TOKEN(I32, \""  << tok.text << "\")"; break;
        case Token::I64:  outs << "TOKEN(I64, \""  << tok.text << "\")"; break;
        case Token::F32:  outs << "TOKEN(F32, \""  << tok.text << "\")"; break;
        case Token::F64:  outs << "TOKEN(F64, \""  << tok.text << "\")"; break;
        case Token::BOOL: outs << "TOKEN(BOOL, \"" << tok.text << "\")"; break;
        
        // Literales booleanos
        case Token::TRUE:  outs << "TOKEN(TRUE, \""  << tok.text << "\")"; break;
        case Token::FALSE: outs << "TOKEN(FALSE, \"" << tok.text << "\")"; break;
        
        // Conversión
        case Token::AS:     outs << "TOKEN(AS, \""     << tok.text << "\")"; break;
        case Token::UNSAFE: outs << "TOKEN(UNSAFE, \"" << tok.text << "\")"; break;
        
        // Macros
        case Token::PRINTLN: outs << "TOKEN(PRINTLN, \"" << tok.text << "\")"; break;
        
        // Literales
        case Token::NUM:    outs << "TOKEN(NUM, \""    << tok.text << "\")"; break;
        case Token::FLOAT:  outs << "TOKEN(FLOAT, \""  << tok.text << "\")"; break;
        case Token::STRING: outs << "TOKEN(STRING, \"" << tok.text << "\")"; break;
        case Token::ID:     outs << "TOKEN(ID, \""     << tok.text << "\")"; break;
        
        // Tokens obsoletos (compatibilidad)
        case Token::SQRT:     outs << "TOKEN(SQRT, \""     << tok.text << "\")"; break;
        case Token::FUN:      outs << "TOKEN(FUN, \""      << tok.text << "\")"; break;
        case Token::ENDFUN:   outs << "TOKEN(ENDFUN, \""   << tok.text << "\")"; break;
        case Token::PRINT:    outs << "TOKEN(PRINT, \""    << tok.text << "\")"; break;
        case Token::DO:       outs << "TOKEN(DO, \""       << tok.text << "\")"; break;
        case Token::THEN:     outs << "TOKEN(THEN, \""     << tok.text << "\")"; break;
        case Token::ENDIF:    outs << "TOKEN(ENDIF, \""    << tok.text << "\")"; break;
        case Token::ENDWHILE: outs << "TOKEN(ENDWHILE, \"" << tok.text << "\")"; break;
        case Token::VAR:      outs << "TOKEN(VAR, \""      << tok.text << "\")"; break;
        
        // Control
        case Token::ERR: outs << "TOKEN(ERR, \"" << tok.text << "\")"; break;
        case Token::END: outs << "TOKEN(END)"; break;
        
        default: outs << "TOKEN(UNKNOWN, \"" << tok.text << "\")"; break;
    }
    
    if (tok.line > 0) {
        outs << " @(" << tok.line << ":" << tok.column << ")";
    }
    return outs;
}

// Para Token puntero
ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;  // delega al otro
}