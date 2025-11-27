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

class TypeKindVisitor : public Visitor {
public:
    bool isBaseType = false;
    bool isPointerType = false;
    BaseTypeKind baseKind;
    Type* pointeeType = nullptr;
    
    int visit(BaseType* type) override {
        isBaseType = true;
        baseKind = type->kind;
        return 0;
    }
    int visit(PointerType* type) override {
        isPointerType = true;
        pointeeType = type->pointeeType;
        return 0;
    }
    

    int visit(BinaryExp*) override { return 0; }
    int visit(UnaryExp*) override { return 0; }
    int visit(NumberExp*) override { return 0; }
    int visit(FloatExp*) override { return 0; }
    int visit(BoolExp*) override { return 0; }
    int visit(StringExp*) override { return 0; }
    int visit(IdExp*) override { return 0; }
    int visit(ParenExp*) override { return 0; }
    int visit(FunCallExp*) override { return 0; }
    int visit(CastExp*) override { return 0; }
    int visit(IfExp*) override { return 0; }
    int visit(Block*) override { return 0; }
    int visit(VarDeclStm*) override { return 0; }
    int visit(AssignStm*) override { return 0; }
    int visit(PrintlnStm*) override { return 0; }
    int visit(IfStm*) override { return 0; }
    int visit(WhileStm*) override { return 0; }
    int visit(ForRangeStm*) override { return 0; }
    int visit(ReturnStm*) override { return 0; }
    int visit(ExprStm*) override { return 0; }
    int visit(GlobalVarDecl*) override { return 0; }
    int visit(FunDecl*) override { return 0; }
    int visit(Program*) override { return 0; }
};

class GenCodeVisitor : public Visitor {
private:
    ostream& out;
    unordered_map<string, int> localVars;    // offset desde %rbp
    unordered_map<string, bool> globalVars;  // variables globales
    unordered_map<string, Type*> varTypes;   // tipos de variables
    int stackOffset;
    int labelCounter;
    bool inFunction;
    string currentFunction;
    Type* lastExprType;  // tipo de la última expresión evaluada
    bool lastExprIsFloat;  // true si última expr está en XMM, false si en GPR
    bool usesFloats;  // true si el programa usa flotantes 
    
    // Función auxiliar para contar variables locales
    void countLocalVars(Block* block);
    void countLocalVarsInStmt(Stm* stmt);
    
    // Pre-pasada para detectar flotantes
    void detectFloats(Program* prog);
    void detectFloatsInBlock(Block* block);
    void detectFloatsInStmt(Stm* stmt);
    void detectFloatsInExp(Exp* exp);
    
    // Optimización: evaluar expresiones constantes
    bool tryEvalConst(Exp* exp, long long& result);
    

    bool isFloatType(Type* type);
    bool isPointerType(Type* type);
    BaseTypeKind getBaseTypeKind(Type* type);
    Type* getPointeeType(Type* type);
    
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