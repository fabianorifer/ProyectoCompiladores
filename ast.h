#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <vector>
#include <ostream>
using namespace std;

class Visitor;

// Forward declarations
class IdExp;
class BinaryExp;
class VarDeclStm;
class Block;
class IfStm;
class WhileStm;
class ForRangeStm;
class ExprStm;

// ============================================
// TIPOS
// ============================================

enum BaseTypeKind {
    I32_TYPE,
    I64_TYPE,
    F32_TYPE,
    F64_TYPE,
    BOOL_TYPE
};

class Type {
public:
    virtual ~Type() {}
    virtual int accept(Visitor* visitor) = 0;
    virtual string toString() = 0;
};

class BaseType : public Type {
public:
    BaseTypeKind kind;
    BaseType(BaseTypeKind k) : kind(k) {}
    int accept(Visitor* visitor);
    string toString();
    ~BaseType() {}
};

class PointerType : public Type {
public:
    bool isMutable;  // true = *mut, false = *const
    Type* pointeeType;
    PointerType(bool mut, Type* pt) : isMutable(mut), pointeeType(pt) {}
    int accept(Visitor* visitor);
    string toString();
    ~PointerType() { delete pointeeType; }
};

// ============================================
// OPERADORES
// ============================================

enum BinaryOp {
    // Aritméticos
    PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, MOD_OP, POW_OP,
    // Relacionales
    LT_OP, LE_OP, GT_OP, GE_OP, EQ_OP, NE_OP,
    // Lógicos
    AND_OP, OR_OP
};

enum UnaryOp {
    NOT_OP,        // !
    NEG_OP,        // -
    DEREF_OP,      // *
    REF_OP,        // &
    REF_MUT_OP     // &mut
};

enum AssignOp {
    ASSIGN,        // =
    PLUS_ASSIGN,   // +=
    MINUS_ASSIGN,  // -=
    MUL_ASSIGN,    // *=
    DIV_ASSIGN,    // /=
    MOD_ASSIGN     // %=
};

// ============================================
// EXPRESIONES
// ============================================

class Exp {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Exp() {}
    virtual IdExp* asIdExp() { return nullptr; }
    virtual BinaryExp* asBinaryExp() { return nullptr; }
    virtual Exp* optimize() { return this; }  // Por defecto retorna this (sin cambios)
    virtual bool isConstant() { return false; }
    virtual long long getConstValue() { return 0; }
    static string binopToString(BinaryOp op);
    static string unopToString(UnaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp o) : left(l), right(r), op(o) {}
    int accept(Visitor* visitor);
    BinaryExp* asBinaryExp() override { return this; }
    Exp* optimize() override;
    ~BinaryExp() { delete left; delete right; }
};

class UnaryExp : public Exp {
public:
    UnaryOp op;
    Exp* operand;
    UnaryExp(UnaryOp o, Exp* e) : op(o), operand(e) {}
    int accept(Visitor* visitor);
    ~UnaryExp() { delete operand; }
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v) : value(v) {}
    int accept(Visitor* visitor);
    bool isConstant() override { return true; }
    long long getConstValue() override { return value; }
    ~NumberExp() {}
};

class FloatExp : public Exp {
public:
    double value;
    FloatExp(double v) : value(v) {}
    int accept(Visitor* visitor);
    ~FloatExp() {}
};

class BoolExp : public Exp {
public:
    bool value;
    BoolExp(bool v) : value(v) {}
    int accept(Visitor* visitor);
    ~BoolExp() {}
};

class StringExp : public Exp {
public:
    string value;
    StringExp(string v) : value(v) {}
    int accept(Visitor* visitor);
    ~StringExp() {}
};

class IdExp : public Exp {
public:
    string id;
    IdExp(string i) : id(i) {}
    int accept(Visitor* visitor);
    IdExp* asIdExp() override { return this; }
    ~IdExp() {}
};

class ParenExp : public Exp {
public:
    Exp* inner;
    ParenExp(Exp* e) : inner(e) {}
    int accept(Visitor* visitor);
    Exp* optimize() override { 
        // Optimizar la expresión interna y retornarla directamente
        // (los paréntesis ya no son necesarios en el AST optimizado)
        return inner->optimize(); 
    }
    ~ParenExp() { delete inner; }
};

class FunCallExp : public Exp {
public:
    string funName;
    vector<Exp*> args;
    FunCallExp(string name) : funName(name) {}
    int accept(Visitor* visitor);
    ~FunCallExp() {
        for (auto arg : args) delete arg;
    }
};

class CastExp : public Exp {
public:
    Exp* expr;
    Type* targetType;
    CastExp(Exp* e, Type* t) : expr(e), targetType(t) {}
    int accept(Visitor* visitor);
    ~CastExp() { delete expr; delete targetType; }
};

class IfExp : public Exp {
public:
    Exp* condition;
    class Block* thenBlock;
    class Block* elseBlock;  // puede ser nullptr
    IfExp(Exp* c, class Block* t, class Block* e = nullptr) 
        : condition(c), thenBlock(t), elseBlock(e) {}
    int accept(Visitor* visitor);
    ~IfExp();
};

// ============================================
// STATEMENTS
// ============================================

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() {}
    virtual VarDeclStm* asVarDeclStm() { return nullptr; }
    virtual Block* asBlock() { return nullptr; }
    virtual IfStm* asIfStm() { return nullptr; }
    virtual WhileStm* asWhileStm() { return nullptr; }
    virtual ForRangeStm* asForRangeStm() { return nullptr; }
    virtual ExprStm* asExprStm() { return nullptr; }
};

class Block : public Stm {
public:
    vector<Stm*> stmts;
    Block() {}
    int accept(Visitor* visitor);
    Block* asBlock() override { return this; }
    ~Block() {
        for (auto s : stmts) delete s;
    }
};

class VarDeclStm : public Stm {
public:
    bool isMutable;
    string varName;
    Type* varType;       // puede ser nullptr (inferencia)
    Exp* initializer;    // puede ser nullptr
    VarDeclStm(bool mut, string name, Type* t, Exp* init)
        : isMutable(mut), varName(name), varType(t), initializer(init) {}
    int accept(Visitor* visitor);
    VarDeclStm* asVarDeclStm() override { return this; }
    ~VarDeclStm() {
        if (varType) delete varType;
        if (initializer) delete initializer;
    }
};

class AssignStm : public Stm {
public:
    Exp* lhs;        // puede ser IdExp o DerefExp
    AssignOp op;
    Exp* rhs;
    AssignStm(Exp* l, AssignOp o, Exp* r) : lhs(l), op(o), rhs(r) {}
    int accept(Visitor* visitor);
    ~AssignStm() { delete lhs; delete rhs; }
};

class PrintlnStm : public Stm {
public:
    string formatString;  // puede estar vacío
    vector<Exp*> args;
    PrintlnStm(string fmt) : formatString(fmt) {}
    int accept(Visitor* visitor);
    ~PrintlnStm() {
        for (auto e : args) delete e;
    }
};

class IfStm : public Stm {
public:
    Exp* condition;
    Block* thenBlock;
    Block* elseBlock;  // puede ser nullptr
    IfStm(Exp* c, Block* t, Block* e = nullptr)
        : condition(c), thenBlock(t), elseBlock(e) {}
    int accept(Visitor* visitor);
    IfStm* asIfStm() override { return this; }
    ~IfStm() {
        delete condition;
        delete thenBlock;
        if (elseBlock) delete elseBlock;
    }
};

class WhileStm : public Stm {
public:
    Exp* condition;
    Block* body;
    WhileStm(Exp* c, Block* b) : condition(c), body(b) {}
    int accept(Visitor* visitor);
    WhileStm* asWhileStm() override { return this; }
    ~WhileStm() {
        delete condition;
        delete body;
    }
};

class ForRangeStm : public Stm {
public:
    string loopVar;
    Exp* rangeStart;
    Exp* rangeEnd;
    Block* body;
    ForRangeStm(string var, Exp* start, Exp* end, Block* b)
        : loopVar(var), rangeStart(start), rangeEnd(end), body(b) {}
    int accept(Visitor* visitor);
    ForRangeStm* asForRangeStm() override { return this; }
    ~ForRangeStm() {
        delete rangeStart;
        delete rangeEnd;
        delete body;
    }
};

class ReturnStm : public Stm {
public:
    Exp* returnValue;  // puede ser nullptr
    ReturnStm(Exp* e = nullptr) : returnValue(e) {}
    int accept(Visitor* visitor);
    ~ReturnStm() {
        if (returnValue) delete returnValue;
    }
};

class ExprStm : public Stm {
public:
    Exp* expr;
    ExprStm(Exp* e) : expr(e) {}
    int accept(Visitor* visitor);
    ExprStm* asExprStm() override { return this; }
    ~ExprStm() { delete expr; }
};

// ============================================
// DECLARACIONES
// ============================================

class Param {
public:
    string name;
    Type* type;
    Param(string n, Type* t) : name(n), type(t) {}
    ~Param() { delete type; }
};

class FunDecl {
public:
    string name;
    vector<Param*> params;
    Type* returnType;  // nullptr = sin tipo de retorno
    Block* body;
    FunDecl(string n) : name(n), returnType(nullptr), body(nullptr) {}
    int accept(Visitor* visitor);
    ~FunDecl() {
        for (auto p : params) delete p;
        if (returnType) delete returnType;
        if (body) delete body;
    }
};

class GlobalVarDecl {
public:
    bool isMutable;
    string varName;
    Type* varType;       // puede ser nullptr (inferencia)
    Exp* initializer;    // puede ser nullptr
    GlobalVarDecl(bool mut, string name, Type* t, Exp* init)
        : isMutable(mut), varName(name), varType(t), initializer(init) {}
    int accept(Visitor* visitor);
    ~GlobalVarDecl() {
        if (varType) delete varType;
        if (initializer) delete initializer;
    }
};

// ============================================
// PROGRAMA
// ============================================

class Program {
public:
    vector<GlobalVarDecl*> globalVars;
    vector<FunDecl*> functions;
    Program() {}
    int accept(Visitor* visitor);
    ~Program() {
        for (auto gv : globalVars) delete gv;
        for (auto f : functions) delete f;
    }
};

#endif // AST_H
