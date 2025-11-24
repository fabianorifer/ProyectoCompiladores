#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include <unordered_map>
#include <string>
#include <ostream>

using namespace std;

class Visitor {
public:
    virtual ~Visitor() {}
    
    // Tipos
    virtual int visit(BaseType* type) = 0;
    virtual int visit(PointerType* type) = 0;
    
    // Expresiones
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(UnaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(FloatExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(StringExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(ParenExp* exp) = 0;
    virtual int visit(FunCallExp* exp) = 0;
    virtual int visit(CastExp* exp) = 0;
    virtual int visit(IfExp* exp) = 0;
    
    // Statements
    virtual int visit(Block* block) = 0;
    virtual int visit(VarDeclStm* stm) = 0;
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(PrintlnStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(ForRangeStm* stm) = 0;
    virtual int visit(ReturnStm* stm) = 0;
    virtual int visit(ExprStm* stm) = 0;
    
    // Declaraciones
    virtual int visit(GlobalVarDecl* decl) = 0;
    virtual int visit(FunDecl* decl) = 0;
    virtual int visit(Program* prog) = 0;
};

class GenCodeVisitor : public Visitor {
private:
    ostream& out;
    unordered_map<string, int> localVars;    // offset desde %rbp
    unordered_map<string, bool> globalVars;  // variables globales
    int stackOffset;
    int labelCounter;
    bool inFunction;
    string currentFunction;
    
public:
    GenCodeVisitor(ostream& output);
    int generar(Program* program);
    
    // Tipos
    int visit(BaseType* type) override;
    int visit(PointerType* type) override;
    
    // Expresiones
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(ParenExp* exp) override;
    int visit(FunCallExp* exp) override;
    int visit(CastExp* exp) override;
    int visit(IfExp* exp) override;
    
    // Statements
    int visit(Block* block) override;
    int visit(VarDeclStm* stm) override;
    int visit(AssignStm* stm) override;
    int visit(PrintlnStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForRangeStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(ExprStm* stm) override;
    
    // Declaraciones
    int visit(GlobalVarDecl* decl) override;
    int visit(FunDecl* decl) override;
    int visit(Program* prog) override;
};

#endif // VISITOR_H