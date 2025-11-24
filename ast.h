// Actualizado para nueva gramática estilo Rust manteniendo patrón Visitor
#ifndef AST_RUST_H
#define AST_RUST_H

#include <string>
#include <vector>
#include <list>
using namespace std;

class Visitor;

// Tipos soportados (ampliable a punteros si se requiere)
enum class DataType { VOID, I32, I64, F32, F64, BOOL, UNKNOWN };

// Operadores binarios
enum class BinaryOp {
    PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, MOD_OP, POW_OP,
    OR_OP, AND_OP,
    EQ_OP, NE_OP,
    LT_OP, LE_OP, GT_OP, GE_OP
};

// Operadores unarios
enum class UnaryOp { MINUS_OP, NOT_OP, DEREF_OP, ADDR_OP, ADDR_MUT_OP };

// Clase base Node
class Node {
public:
    virtual int accept(Visitor* v) = 0;
    virtual ~Node() {}
};

// Expresiones
class Exp : public Node { };
class Stm : public Node { };

class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* v);
};

class UnaryExp : public Exp {
public:
    UnaryOp op;
    Exp* operand;
    UnaryExp(UnaryOp op, Exp* operand);
    int accept(Visitor* v);
};

class NumberExp : public Exp {
public:
    string value;        // Mantener string para soportar flotantes/notación científica
    DataType numberType; // Tipo explícito según contexto
    NumberExp(string v, DataType t);
    int accept(Visitor* v);
};

class BoolExp : public Exp {
public:
    bool value;
    BoolExp(bool v);
    int accept(Visitor* v);
};

class StringExp : public Exp {
public:
    string value;
    StringExp(string v);
    int accept(Visitor* v);
};

class IdExp : public Exp {
public:
    string id;
    IdExp(string v);
    int accept(Visitor* v);
};

class CallExp : public Exp {
public:
    string funcName;
    vector<Exp*> args;
    CallExp(string name, vector<Exp*> args);
    int accept(Visitor* v);
};

class CastExp : public Exp {
public:
    Exp* expr;
    DataType targetType;
    CastExp(Exp* e, DataType t);
    int accept(Visitor* v);
};

// Sentencias y bloques
class Block : public Stm {
public:
    vector<Stm*> stmts;
    Block();
    void addStm(Stm* s);
    int accept(Visitor* v);
};

class VarDec : public Stm { // También puede ser ítem global
public:
    string name;
    DataType type;
    Exp* init;
    bool isMut;
    bool isGlobal;
    VarDec(string n, DataType t, Exp* i, bool m, bool g);
    int accept(Visitor* v);
};

class AssignStm : public Stm {
public:
    Exp* place; // normalmente IdExp o deref futuro
    Exp* expr;
    AssignStm(Exp* p, Exp* e);
    int accept(Visitor* v);
};

class IfStm : public Stm {
public:
    Exp* condition;
    Block* thenBlock;
    Stm* elseBlock; // Block o If encadenado
    IfStm(Exp* c, Block* t, Stm* e);
    int accept(Visitor* v);
};

class WhileStm : public Stm {
public:
    Exp* condition;
    Block* block;
    WhileStm(Exp* c, Block* b);
    int accept(Visitor* v);
};

class ForStm : public Stm {
public:
    string var;
    Exp* start;
    Exp* end;
    Block* block;
    ForStm(string v, Exp* s, Exp* e, Block* b);
    int accept(Visitor* v);
};

class ReturnStm : public Stm {
public:
    Exp* expr; // opcional
    ReturnStm(Exp* e);
    int accept(Visitor* v);
};

class PrintStm : public Stm {
public:
    string format;       // cadena de formato (puede vacía)
    vector<Exp*> args;   // argumentos opcionales
    PrintStm(string f, vector<Exp*> a);
    int accept(Visitor* v);
};

class ExprStm : public Stm {
public:
    Exp* expr;
    ExprStm(Exp* e);
    int accept(Visitor* v);
};

class FunDec : public Node {
public:
    string name;
    vector<pair<string, DataType>> params;
    DataType returnType;
    Block* body;
    FunDec(string n, vector<pair<string, DataType>> p, DataType rt, Block* b);
    int accept(Visitor* v);
};

class Program : public Node {
public:
    vector<Node*> items; // VarDec globales o FunDec
    void addItem(Node* item);
    int accept(Visitor* v);
};

#endif // AST_RUST_H
