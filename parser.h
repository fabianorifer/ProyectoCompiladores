#ifndef PARSER_H       
#define PARSER_H

#include "scanner.h"
#include "ast.h"

class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    
    // Métodos auxiliares
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    
    // Parsing de tipos
    Type* parseType();
    Type* parseBaseType();
    Type* parsePointerType();
    
    // Parsing de expresiones (según precedencia)
    Exp* parseExpr();              // Punto de entrada: IfExpr | LogicOr
    Exp* parseIfExpr();            // if expr
    Exp* parseLogicOr();           // ||
    Exp* parseLogicAnd();          // &&
    Exp* parseEquality();          // == !=
    Exp* parseRelational();        // < <= > >=
    Exp* parseAdditive();          // + -
    Exp* parseMultiplicative();    // * / %
    Exp* parsePower();             // **
    Exp* parsePrefix();            // ! - & &mut *
    Exp* parseCast();              // as Type
    Exp* parsePostfix();           // function calls
    Exp* parsePrimary();           // literals, ID, (expr)
    
    // Parsing de statements
    Stm* parseStmt();
    Stm* parseSimpleStmt();
    VarDeclStm* parseVarDecl();
    AssignStm* parseAssignStmt();
    ReturnStm* parseReturnStmt();
    ExprStm* parseExprStmt();
    IfStm* parseIfStmt();
    WhileStm* parseWhileStmt();
    ForRangeStm* parseForRangeStmt();
    PrintlnStm* parsePrintlnStmt();
    Block* parseBlock();
    
    // Parsing de declaraciones top-level
    GlobalVarDecl* parseGlobalVarDecl();
    FunDecl* parseFunDecl();
    Param* parseParam();
    
public:
    Parser(Scanner* sc);
    Program* parseProgram();
};

#endif // PARSER_H      