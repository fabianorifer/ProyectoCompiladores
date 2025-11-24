#include <iostream>
#include "token.h"

using namespace std;

// -----------------------------
// Constructores
// -----------------------------

Token::Token(Type type) 
    : type(type), text("") { }

Token::Token(Type type, char c) 
    : type(type), text(string(1, c)) { }

Token::Token(Type type, const string& source, int first, int last) 
    : type(type), text(source.substr(first, last)) { }

// -----------------------------
// Sobrecarga de operador <<
// -----------------------------

// Para Token por referencia
ostream& operator<<(ostream& outs, const Token& tok) {
    switch (tok.type) {
        // Operadores aritméticos
        case Token::PLUS:   outs << "TOKEN(PLUS, \"" << tok.text << "\")"; break;
        case Token::MINUS:  outs << "TOKEN(MINUS, \"" << tok.text << "\")"; break;
        case Token::MUL:    outs << "TOKEN(MUL, \"" << tok.text << "\")"; break;
        case Token::DIV:    outs << "TOKEN(DIV, \"" << tok.text << "\")"; break;
        case Token::MOD:    outs << "TOKEN(MOD, \"" << tok.text << "\")"; break;
        case Token::POW:    outs << "TOKEN(POW, \"" << tok.text << "\")"; break;
        
        // Operadores de comparación
        case Token::EQ:     outs << "TOKEN(EQ, \"" << tok.text << "\")"; break;
        case Token::NEQ:    outs << "TOKEN(NEQ, \"" << tok.text << "\")"; break;
        case Token::LT:     outs << "TOKEN(LT, \"" << tok.text << "\")"; break;
        case Token::LE:     outs << "TOKEN(LE, \"" << tok.text << "\")"; break;
        case Token::GT:     outs << "TOKEN(GT, \"" << tok.text << "\")"; break;
        case Token::GE:     outs << "TOKEN(GE, \"" << tok.text << "\")"; break;
        
        // Operadores lógicos
        case Token::AND:    outs << "TOKEN(AND, \"" << tok.text << "\")"; break;
        case Token::OR:     outs << "TOKEN(OR, \"" << tok.text << "\")"; break;
        case Token::NOT:    outs << "TOKEN(NOT, \"" << tok.text << "\")"; break;
        
        // Operadores de asignación
        case Token::ASSIGN:       outs << "TOKEN(ASSIGN, \"" << tok.text << "\")"; break;
        case Token::PLUS_ASSIGN:  outs << "TOKEN(PLUS_ASSIGN, \"" << tok.text << "\")"; break;
        case Token::MINUS_ASSIGN: outs << "TOKEN(MINUS_ASSIGN, \"" << tok.text << "\")"; break;
        case Token::MUL_ASSIGN:   outs << "TOKEN(MUL_ASSIGN, \"" << tok.text << "\")"; break;
        case Token::DIV_ASSIGN:   outs << "TOKEN(DIV_ASSIGN, \"" << tok.text << "\")"; break;
        case Token::MOD_ASSIGN:   outs << "TOKEN(MOD_ASSIGN, \"" << tok.text << "\")"; break;
        
        // Delimitadores
        case Token::LPAREN:  outs << "TOKEN(LPAREN, \"" << tok.text << "\")"; break;
        case Token::RPAREN:  outs << "TOKEN(RPAREN, \"" << tok.text << "\")"; break;
        case Token::LBRACE:  outs << "TOKEN(LBRACE, \"" << tok.text << "\")"; break;
        case Token::RBRACE:  outs << "TOKEN(RBRACE, \"" << tok.text << "\")"; break;
        case Token::SEMICOL: outs << "TOKEN(SEMICOL, \"" << tok.text << "\")"; break;
        case Token::COMA:    outs << "TOKEN(COMA, \"" << tok.text << "\")"; break;
        case Token::COLON:   outs << "TOKEN(COLON, \"" << tok.text << "\")"; break;
        case Token::ARROW:   outs << "TOKEN(ARROW, \"" << tok.text << "\")"; break;
        case Token::DOTDOT:  outs << "TOKEN(DOTDOT, \"" << tok.text << "\")"; break;
        
        // Keywords
        case Token::LET:     outs << "TOKEN(LET, \"" << tok.text << "\")"; break;
        case Token::MUT:     outs << "TOKEN(MUT, \"" << tok.text << "\")"; break;
        case Token::FN:      outs << "TOKEN(FN, \"" << tok.text << "\")"; break;
        case Token::RETURN:  outs << "TOKEN(RETURN, \"" << tok.text << "\")"; break;
        case Token::IF:      outs << "TOKEN(IF, \"" << tok.text << "\")"; break;
        case Token::ELSE:    outs << "TOKEN(ELSE, \"" << tok.text << "\")"; break;
        case Token::WHILE:   outs << "TOKEN(WHILE, \"" << tok.text << "\")"; break;
        case Token::FOR:     outs << "TOKEN(FOR, \"" << tok.text << "\")"; break;
        case Token::IN:      outs << "TOKEN(IN, \"" << tok.text << "\")"; break;
        case Token::AS:      outs << "TOKEN(AS, \"" << tok.text << "\")"; break;
        case Token::PRINTLN: outs << "TOKEN(PRINTLN, \"" << tok.text << "\")"; break;
        
        // Tipos
        case Token::I32:     outs << "TOKEN(I32, \"" << tok.text << "\")"; break;
        case Token::I64:     outs << "TOKEN(I64, \"" << tok.text << "\")"; break;
        case Token::F32:     outs << "TOKEN(F32, \"" << tok.text << "\")"; break;
        case Token::F64:     outs << "TOKEN(F64, \"" << tok.text << "\")"; break;
        case Token::BOOL:    outs << "TOKEN(BOOL, \"" << tok.text << "\")"; break;
        
        // Punteros
        case Token::PTR_MUT:   outs << "TOKEN(PTR_MUT, \"" << tok.text << "\")"; break;
        case Token::PTR_CONST: outs << "TOKEN(PTR_CONST, \"" << tok.text << "\")"; break;
        case Token::AMP:       outs << "TOKEN(AMP, \"" << tok.text << "\")"; break;
        case Token::AMP_MUT:   outs << "TOKEN(AMP_MUT, \"" << tok.text << "\")"; break;
        case Token::DEREF:     outs << "TOKEN(DEREF, \"" << tok.text << "\")"; break;
        
        // Literales y otros
        case Token::NUM:    outs << "TOKEN(NUM, \"" << tok.text << "\")"; break;
        case Token::FLOAT:  outs << "TOKEN(FLOAT, \"" << tok.text << "\")"; break;
        case Token::STRING: outs << "TOKEN(STRING, \"" << tok.text << "\")"; break;
        case Token::TRUE:   outs << "TOKEN(TRUE, \"" << tok.text << "\")"; break;
        case Token::FALSE:  outs << "TOKEN(FALSE, \"" << tok.text << "\")"; break;
        case Token::ID:     outs << "TOKEN(ID, \"" << tok.text << "\")"; break;
        
        // Control
        case Token::ERR:    outs << "TOKEN(ERR, \"" << tok.text << "\")"; break;
        case Token::END:    outs << "TOKEN(END)"; break;
    }
    return outs;
}

// Para Token puntero
ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;  // delega al otro
}