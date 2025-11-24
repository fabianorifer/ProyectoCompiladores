#include "ast.h"
#include "visitor.h"

// BinaryExp
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op) : left(l), right(r), op(op) {}
int BinaryExp::accept(Visitor* visitor) { return visitor->visit(this); }

// UnaryExp
UnaryExp::UnaryExp(UnaryOp op, Exp* operand) : op(op), operand(operand) {}
int UnaryExp::accept(Visitor* visitor) { return visitor->visit(this); }

// NumberExp
NumberExp::NumberExp(string v, DataType t) : value(v), numberType(t) {}
int NumberExp::accept(Visitor* visitor) { return visitor->visit(this); }

// BoolExp
BoolExp::BoolExp(bool v) : value(v) {}
int BoolExp::accept(Visitor* visitor) { return visitor->visit(this); }

// StringExp
StringExp::StringExp(string v) : value(v) {}
int StringExp::accept(Visitor* visitor) { return visitor->visit(this); }

// IdExp
IdExp::IdExp(string v) : id(v) {}
int IdExp::accept(Visitor* visitor) { return visitor->visit(this); }

// CallExp
CallExp::CallExp(string name, vector<Exp*> args) : funcName(name), args(args) {}
int CallExp::accept(Visitor* visitor) { return visitor->visit(this); }

// CastExp
CastExp::CastExp(Exp* e, DataType t) : expr(e), targetType(t) {}
int CastExp::accept(Visitor* visitor) { return visitor->visit(this); }

// Block
Block::Block() {}
void Block::addStm(Stm* s) { stmts.push_back(s); }
int Block::accept(Visitor* visitor) { return visitor->visit(this); }

// VarDec
VarDec::VarDec(string n, DataType t, Exp* i, bool m, bool g) 
    : name(n), type(t), init(i), isMut(m), isGlobal(g) {}
int VarDec::accept(Visitor* visitor) { return visitor->visit(this); }

// AssignStm
AssignStm::AssignStm(Exp* p, Exp* e) : place(p), expr(e) {}
int AssignStm::accept(Visitor* visitor) { return visitor->visit(this); }

// IfStm
IfStm::IfStm(Exp* c, Block* t, Stm* e) : condition(c), thenBlock(t), elseBlock(e) {}
int IfStm::accept(Visitor* visitor) { return visitor->visit(this); }

// WhileStm
WhileStm::WhileStm(Exp* c, Block* b) : condition(c), block(b) {}
int WhileStm::accept(Visitor* visitor) { return visitor->visit(this); }

// ForStm
ForStm::ForStm(string v, Exp* s, Exp* e, Block* b) 
    : var(v), start(s), end(e), block(b) {}
int ForStm::accept(Visitor* visitor) { return visitor->visit(this); }

// ReturnStm
ReturnStm::ReturnStm(Exp* e) : expr(e) {}
int ReturnStm::accept(Visitor* visitor) { return visitor->visit(this); }

// PrintStm
PrintStm::PrintStm(string f, vector<Exp*> a) : format(f), args(a) {}
int PrintStm::accept(Visitor* visitor) { return visitor->visit(this); }

// ExprStm
ExprStm::ExprStm(Exp* e) : expr(e) {}
int ExprStm::accept(Visitor* visitor) { return visitor->visit(this); }

// FunDec
FunDec::FunDec(string n, vector<pair<string, DataType>> p, DataType rt, Block* b)
    : name(n), params(p), returnType(rt), body(b) {}
int FunDec::accept(Visitor* visitor) { return visitor->visit(this); }

// Program
void Program::addItem(Node* item) { items.push_back(item); }
int Program::accept(Visitor* visitor) { return visitor->visit(this); }
