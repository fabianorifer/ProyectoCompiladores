#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

using namespace std;

class BinaryExp;
class UnaryExp;
class NumberExp;
class BoolExp;
class StringExp;
class IdExp;
class CallExp;
class CastExp;
class Block;
class VarDec;
class AssignStm;
class IfStm;
class WhileStm;
class ForStm;
class ReturnStm;
class PrintStm;
class ExprStm;
class FunDec;
class Program;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(UnaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(StringExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(CallExp* exp) = 0;
    virtual int visit(CastExp* exp) = 0;
    virtual int visit(Block* stm) = 0;
    virtual int visit(VarDec* stm) = 0;
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(ForStm* stm) = 0;
    virtual int visit(ReturnStm* stm) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(ExprStm* stm) = 0;
    virtual int visit(FunDec* stm) = 0;
    virtual int visit(Program* stm) = 0;
};

class GenCodeVisitor : public Visitor {
private:
    ostream& out;
    int labelCounter = 0; // para generar etiquetas únicas
    // Tablas de símbolos simples
    struct VarInfo { bool global; int offset; DataType type; }; // incluir tipo para detectar floats
    std::unordered_map<std::string, VarInfo> vars;
    int currentStackSize = 0; // bytes reservados en función actual
    std::string currentFunctionEndLabel; // etiqueta de epílogo actual
    int scratchOffset = 0; // offset negativo reservado para scratch persistente
    int blockNesting = 0; // nivel de anidación de bloques para gestionar liberación local
public:
    GenCodeVisitor(ostream& out) : out(out) {}
    void generar(Program* p); // Método auxiliar para iniciar generación
    
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(CallExp* exp) override;
    int visit(CastExp* exp) override;
    int visit(Block* stm) override;
    int visit(VarDec* stm) override;
    int visit(AssignStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(ExprStm* stm) override;
    int visit(FunDec* stm) override;
    int visit(Program* stm) override;
};

#endif // VISITOR_H
