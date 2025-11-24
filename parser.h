#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "ast.h"
#include <vector>
#include <string>

class Parser {
private:
    Scanner* scanner;
    Token* current;
    Token* previous;
    bool inFunction = false; // para retorno implícito
    bool debug = false; // modo depuración

    bool match(Token::Type t);
    bool check(Token::Type t);
    bool advance();
    bool isAtEnd();
    Token* consume(Token::Type t, string msg);

    // Declaraciones / ítems
    Node* parseDeclaration();
    FunDec* parseFunDec();
    VarDec* parseVarDec(bool global);

    // Tipos y parámetros
    DataType parseType();
    vector<pair<string, DataType>> parseParams();

    // Bloques y sentencias
    Block* parseBlock();
    Stm* parseStatement();
    IfStm* parseIfStm();
    WhileStm* parseWhileStm();
    ForStm* parseForStm();
    ReturnStm* parseReturnStm();
    PrintStm* parsePrintlnStm();

    // Expresiones (precedencia)
    Exp* parseExpression();
    Exp* parseLogicOr();
    Exp* parseLogicAnd();
    Exp* parseEquality();
    Exp* parseComparison();
    Exp* parseTerm();
    Exp* parseFactor();
    Exp* parsePower();
    Exp* parseUnary();
    Exp* parseCall();
    Exp* parsePrimary();
    vector<Exp*> parseArgs();

public:
    Parser(Scanner* scanner);
    Program* parseProgram();
    void setDebug(bool d) { debug = d; }
};

#endif // PARSER_H