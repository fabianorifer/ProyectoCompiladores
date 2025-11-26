#include "visitor.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// ============================================
// Métodos accept en clases AST
// ============================================

int BaseType::accept(Visitor* v) { return v->visit(this); }
int PointerType::accept(Visitor* v) { return v->visit(this); }

int BinaryExp::accept(Visitor* v) { return v->visit(this); }
int UnaryExp::accept(Visitor* v) { return v->visit(this); }
int NumberExp::accept(Visitor* v) { return v->visit(this); }
int FloatExp::accept(Visitor* v) { return v->visit(this); }
int BoolExp::accept(Visitor* v) { return v->visit(this); }
int StringExp::accept(Visitor* v) { return v->visit(this); }
int IdExp::accept(Visitor* v) { return v->visit(this); }
int ParenExp::accept(Visitor* v) { return v->visit(this); }
int FunCallExp::accept(Visitor* v) { return v->visit(this); }
int CastExp::accept(Visitor* v) { return v->visit(this); }
int IfExp::accept(Visitor* v) { return v->visit(this); }

int Block::accept(Visitor* v) { return v->visit(this); }
int VarDeclStm::accept(Visitor* v) { return v->visit(this); }
int AssignStm::accept(Visitor* v) { return v->visit(this); }
int PrintlnStm::accept(Visitor* v) { return v->visit(this); }
int IfStm::accept(Visitor* v) { return v->visit(this); }
int WhileStm::accept(Visitor* v) { return v->visit(this); }
int ForRangeStm::accept(Visitor* v) { return v->visit(this); }
int ReturnStm::accept(Visitor* v) { return v->visit(this); }
int ExprStm::accept(Visitor* v) { return v->visit(this); }

int GlobalVarDecl::accept(Visitor* v) { return v->visit(this); }
int FunDecl::accept(Visitor* v) { return v->visit(this); }
int Program::accept(Visitor* v) { return v->visit(this); }

// ============================================
// GenCodeVisitor - Constructor
// ============================================

GenCodeVisitor::GenCodeVisitor(ostream& output) 
    : out(output), stackOffset(-8), labelCounter(0), inFunction(false), lastExprType(nullptr) {}

int GenCodeVisitor::generar(Program* program) {
    return program->accept(this);
}

// ============================================
// Tipos
// ============================================

int GenCodeVisitor::visit(BaseType* type) {
    // No genera código directamente
    return 0;
}

int GenCodeVisitor::visit(PointerType* type) {
    // No genera código directamente
    return 0;
}

// ============================================
// Expresiones
// ============================================

int GenCodeVisitor::visit(NumberExp* exp) {
    out << "  movq $" << exp->value << ", %rax\n";
    return 0;
}

int GenCodeVisitor::visit(FloatExp* exp) {
    // TODO: Implement proper float support with XMM registers
    // Por ahora, truncamos a entero
    out << "  # FloatExp " << exp->value << " (simplified as int)\n";
    out << "  movq $" << (long long)exp->value << ", %rax\n";
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << "  movq $" << (exp->value ? 1 : 0) << ", %rax\n";
    return 0;
}

int GenCodeVisitor::visit(StringExp* exp) {
    // TODO: Implementar strings en .data
    out << "  # StringExp \"" << exp->value << "\" (not implemented)\n";
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if (globalVars.count(exp->id)) {
        out << "  movq " << exp->id << "(%rip), %rax\n";
    } else if (localVars.count(exp->id)) {
        out << "  movq " << localVars[exp->id] << "(%rbp), %rax\n";
    } else {
        throw runtime_error("Variable no declarada: " + exp->id);
    }
    return 0;
}

int GenCodeVisitor::visit(ParenExp* exp) {
    return exp->inner->accept(this);
}

// Método auxiliar para evaluar expresiones constantes
bool GenCodeVisitor::tryEvalConst(Exp* exp, long long& result) {
    // Caso 1: NumberExp (literal numérico)
    if (exp->isConstant()) {
        result = exp->getConstValue();
        return true;
    }
    
    // Caso 2: BinaryExp con ambos operandos constantes
    BinaryExp* binExp = exp->asBinaryExp();
    if (binExp) {
        long long left, right;
        if (tryEvalConst(binExp->left, left) && tryEvalConst(binExp->right, right)) {
            switch (binExp->op) {
                case PLUS_OP:  result = left + right; return true;
                case MINUS_OP: result = left - right; return true;
                case MUL_OP:   result = left * right; return true;
                case DIV_OP:   if (right != 0) { result = left / right; return true; } break;
                case MOD_OP:   if (right != 0) { result = left % right; return true; } break;
                case LT_OP:    result = left < right ? 1 : 0; return true;
                case LE_OP:    result = left <= right ? 1 : 0; return true;
                case GT_OP:    result = left > right ? 1 : 0; return true;
                case GE_OP:    result = left >= right ? 1 : 0; return true;
                case EQ_OP:    result = left == right ? 1 : 0; return true;
                case NE_OP:    result = left != right ? 1 : 0; return true;
                case AND_OP:   result = (left && right) ? 1 : 0; return true;
                case OR_OP:    result = (left || right) ? 1 : 0; return true;
                default: break;
            }
        }
    }
    
    return false;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    // Intentar evaluar como constante
    long long constResult;
    if (tryEvalConst(exp, constResult)) {
        out << "  movq $" << constResult << ", %rax\n";
        return 0;
    }
    
    // Caso no constante: generación normal
    exp->left->accept(this);
    out << "  pushq %rax\n";
    exp->right->accept(this);
    out << "  movq %rax, %rcx\n";
    out << "  popq %rax\n";
    
    switch (exp->op) {
        case PLUS_OP:
            out << "  addq %rcx, %rax\n";
            break;
        case MINUS_OP:
            out << "  subq %rcx, %rax\n";
            break;
        case MUL_OP:
            out << "  imulq %rcx, %rax\n";
            break;
        case DIV_OP:
            out << "  cqto\n";
            out << "  idivq %rcx\n";
            break;
        case MOD_OP:
            out << "  cqto\n";
            out << "  idivq %rcx\n";
            out << "  movq %rdx, %rax\n";
            break;
        case POW_OP:
            out << "  # POW not implemented\n";
            break;
            
        // Relacionales
        case LT_OP:
            out << "  cmpq %rcx, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  setl %al\n";
            break;
        case LE_OP:
            out << "  cmpq %rcx, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  setle %al\n";
            break;
        case GT_OP:
            out << "  cmpq %rcx, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  setg %al\n";
            break;
        case GE_OP:
            out << "  cmpq %rcx, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  setge %al\n";
            break;
        case EQ_OP:
            out << "  cmpq %rcx, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  sete %al\n";
            break;
        case NE_OP:
            out << "  cmpq %rcx, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  setne %al\n";
            break;
            
        // Lógicos
        case AND_OP:
            out << "  andq %rcx, %rax\n";
            break;
        case OR_OP:
            out << "  orq %rcx, %rax\n";
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(UnaryExp* exp) {
    exp->operand->accept(this);
    
    switch (exp->op) {
        case NOT_OP:
            out << "  cmpq $0, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  sete %al\n";
            break;
        case NEG_OP:
            out << "  negq %rax\n";
            break;
        case DEREF_OP:
            out << "  movq (%rax), %rax\n";
            break;
        case REF_OP:
        case REF_MUT_OP:
            // TODO: implementar direcciones
            out << "  # REF not implemented\n";
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(FunCallExp* exp) {
    vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    int numArgs = exp->args.size();
    for (int i = 0; i < numArgs && i < 6; i++) {
        exp->args[i]->accept(this);
        out << "  movq %rax, " << argRegs[i] << "\n";
    }
    
    out << "  call " << exp->funName << "\n";
    return 0;
}

int GenCodeVisitor::visit(CastExp* exp) {
    // TODO: Implementar conversiones de tipo
    exp->expr->accept(this);
    out << "  # Cast to " << exp->targetType->toString() << " (not implemented)\n";
    return 0;
}

int GenCodeVisitor::visit(IfExp* exp) {
    int label = labelCounter++;
    
    exp->condition->accept(this);
    out << "  cmpq $0, %rax\n";
    out << "  je .else_" << label << "\n";
    
    exp->thenBlock->accept(this);
    out << "  jmp .endif_" << label << "\n";
    
    out << ".else_" << label << ":\n";
    if (exp->elseBlock) {
        exp->elseBlock->accept(this);
    }
    
    out << ".endif_" << label << ":\n";
    return 0;
}

// ============================================
// Statements
// ============================================

int GenCodeVisitor::visit(Block* block) {
    for (auto stmt : block->stmts) {
        stmt->accept(this);
    }
    return 0;
}

int GenCodeVisitor::visit(VarDeclStm* stm) {
    if (inFunction) {
        // El offset ya fue asignado en countLocalVars, solo generar el inicializador
        if (stm->initializer) {
            stm->initializer->accept(this);
            out << "  movq %rax, " << localVars[stm->varName] << "(%rbp)\n";
        }
    }
    return 0;
}

int GenCodeVisitor::visit(AssignStm* stm) {
    stm->rhs->accept(this);
    
    // LHS debe ser IdExp o DerefExp
    IdExp* idLhs = stm->lhs->asIdExp();
    if (idLhs) {
        if (globalVars.count(idLhs->id)) {
            // Operadores compuestos
            if (stm->op != ASSIGN) {
                out << "  movq %rax, %rcx\n";
                out << "  movq " << idLhs->id << "(%rip), %rax\n";
                // Aplicar operador compuesto
                switch (stm->op) {
                    case PLUS_ASSIGN:  out << "  addq %rcx, %rax\n"; break;
                    case MINUS_ASSIGN: out << "  subq %rcx, %rax\n"; break;
                    case MUL_ASSIGN:   out << "  imulq %rcx, %rax\n"; break;
                    case DIV_ASSIGN:   out << "  cqto\n  idivq %rcx\n"; break;
                    case MOD_ASSIGN:   out << "  cqto\n  idivq %rcx\n  movq %rdx, %rax\n"; break;
                    default: break;
                }
            }
            out << "  movq %rax, " << idLhs->id << "(%rip)\n";
        } else if (localVars.count(idLhs->id)) {
            if (stm->op != ASSIGN) {
                out << "  movq %rax, %rcx\n";
                out << "  movq " << localVars[idLhs->id] << "(%rbp), %rax\n";
                // Aplicar operador compuesto
                switch (stm->op) {
                    case PLUS_ASSIGN:  out << "  addq %rcx, %rax\n"; break;
                    case MINUS_ASSIGN: out << "  subq %rcx, %rax\n"; break;
                    case MUL_ASSIGN:   out << "  imulq %rcx, %rax\n"; break;
                    case DIV_ASSIGN:   out << "  cqto\n  idivq %rcx\n"; break;
                    case MOD_ASSIGN:   out << "  cqto\n  idivq %rcx\n  movq %rdx, %rax\n"; break;
                    default: break;
                }
            }
            out << "  movq %rax, " << localVars[idLhs->id] << "(%rbp)\n";
        }
    }
    return 0;
}

int GenCodeVisitor::visit(PrintlnStm* stm) {
    if (!stm->args.empty()) {
        // Optimizar la expresión antes de generar código
        Exp* optimized = stm->args[0]->optimize();
        optimized->accept(this);
        out << "  movq %rax, %rsi\n";
        out << "  leaq print_fmt(%rip), %rdi\n";
        out << "  movl $0, %eax\n";
        out << "  call printf@PLT\n";
    }
    return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    // Optimización: verificar si la condición es constante
    long long constCond;
    if (tryEvalConst(stm->condition, constCond)) {
        cout << "DEBUG: IfStm optimization triggered, constCond = " << constCond << endl;
        // Condición constante: solo generar la rama tomada
        if (constCond != 0) {
            cout << "DEBUG: Generating ONLY then branch" << endl;
            // Condición true: solo ejecutar then
            stm->thenBlock->accept(this);
        } else {
            cout << "DEBUG: Generating ONLY else branch" << endl;
            // Condición false: solo ejecutar else (si existe)
            if (stm->elseBlock) {
                stm->elseBlock->accept(this);
            }
        }
        return 0;
    }
    
    cout << "DEBUG: condition not constant, generating both branches" << endl;
    // Condición no constante: generación normal con etiquetas
    int label = labelCounter++;
    
    stm->condition->accept(this);
    out << "  cmpq $0, %rax\n";
    out << "  je .else_" << label << "\n";
    
    stm->thenBlock->accept(this);
    out << "  jmp .endif_" << label << "\n";
    
    out << ".else_" << label << ":\n";
    if (stm->elseBlock) {
        stm->elseBlock->accept(this);
    }
    
    out << ".endif_" << label << ":\n";
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int label = labelCounter++;
    
    out << ".while_" << label << ":\n";
    stm->condition->accept(this);
    out << "  cmpq $0, %rax\n";
    out << "  je .endwhile_" << label << "\n";
    
    stm->body->accept(this);
    out << "  jmp .while_" << label << "\n";
    
    out << ".endwhile_" << label << ":\n";
    return 0;
}

int GenCodeVisitor::visit(ForRangeStm* stm) {
    int label = labelCounter++;
    
    // Inicialización: loopVar = rangeStart
    stm->rangeStart->accept(this);
    localVars[stm->loopVar] = stackOffset;
    stackOffset -= 8;
    out << "  movq %rax, " << localVars[stm->loopVar] << "(%rbp)\n";
    
    // Loop
    out << ".for_" << label << ":\n";
    
    // Condición: loopVar < rangeEnd
    out << "  movq " << localVars[stm->loopVar] << "(%rbp), %rax\n";
    out << "  pushq %rax\n";
    stm->rangeEnd->accept(this);
    out << "  movq %rax, %rcx\n";
    out << "  popq %rax\n";
    out << "  cmpq %rcx, %rax\n";
    out << "  jge .endfor_" << label << "\n";
    
    // Cuerpo
    stm->body->accept(this);
    
    // Incremento: loopVar++
    out << "  movq " << localVars[stm->loopVar] << "(%rbp), %rax\n";
    out << "  addq $1, %rax\n";
    out << "  movq %rax, " << localVars[stm->loopVar] << "(%rbp)\n";
    
    out << "  jmp .for_" << label << "\n";
    out << ".endfor_" << label << ":\n";
    
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->returnValue) {
        stm->returnValue->accept(this);
    }
    out << "  jmp .end_" << currentFunction << "\n";
    return 0;
}

int GenCodeVisitor::visit(ExprStm* stm) {
    return stm->expr->accept(this);
}

// ============================================
// Declaraciones
// ============================================

void GenCodeVisitor::countLocalVars(Block* block) {
    for (auto stmt : block->stmts) {
        countLocalVarsInStmt(stmt);
    }
}

void GenCodeVisitor::countLocalVarsInStmt(Stm* stmt) {
    VarDeclStm* varDecl = stmt->asVarDeclStm();
    if (varDecl) {
        if (!localVars.count(varDecl->varName)) {
            localVars[varDecl->varName] = stackOffset;
            stackOffset -= 8;
        }
        return;
    }
    
    Block* block = stmt->asBlock();
    if (block) {
        countLocalVars(block);
        return;
    }
    
    IfStm* ifStm = stmt->asIfStm();
    if (ifStm) {
        countLocalVars(ifStm->thenBlock);
        if (ifStm->elseBlock) {
            countLocalVars(ifStm->elseBlock);
        }
        return;
    }
    
    WhileStm* whileStm = stmt->asWhileStm();
    if (whileStm) {
        countLocalVars(whileStm->body);
        return;
    }
    
    ForRangeStm* forStm = stmt->asForRangeStm();
    if (forStm) {
        if (!localVars.count(forStm->loopVar)) {
            localVars[forStm->loopVar] = stackOffset;
            stackOffset -= 8;
        }
        countLocalVars(forStm->body);
        return;
    }
}

int GenCodeVisitor::visit(GlobalVarDecl* decl) {
    globalVars[decl->varName] = true;
    return 0;
}

int GenCodeVisitor::visit(FunDecl* decl) {
    inFunction = true;
    currentFunction = decl->name;
    localVars.clear();
    stackOffset = -8;
    
    vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    out << ".globl " << decl->name << "\n";
    out << decl->name << ":\n";
    out << "  pushq %rbp\n";
    out << "  movq %rsp, %rbp\n";
    
    // Parámetros
    for (size_t i = 0; i < decl->params.size() && i < 6; i++) {
        localVars[decl->params[i]->name] = stackOffset;
        stackOffset -= 8;
    }
    
    // Pre-pase: contar todas las variables locales
    if (decl->body) {
        countLocalVars(decl->body);
    }
    
    // Reservar espacio en stack ANTES de mover parámetros
    int stackSize = -stackOffset - 8;
    if (stackSize > 0) {
        out << "  subq $" << stackSize << ", %rsp\n";
    }
    
    // Ahora sí mover parámetros a sus posiciones
    for (size_t i = 0; i < decl->params.size() && i < 6; i++) {
        out << "  movq " << argRegs[i] << ", " << localVars[decl->params[i]->name] << "(%rbp)\n";
    }
    
    // Generar cuerpo
    if (decl->body) {
        decl->body->accept(this);
    }
    
    // Retorno por defecto
    out << "  movq $0, %rax\n";
    out << "  jmp .end_" << decl->name << "\n";
    out << ".end_" << decl->name << ":\n";
    out << " leave\n";
    out << " ret\n";
    
    inFunction = false;
    return 0;
}

int GenCodeVisitor::visit(Program* prog) {
    // Sección .data
    out << ".data\n";
    out << "print_fmt: .string \"%ld \\n\"\n";
    
    // Variables globales
    for (auto gv : prog->globalVars) {
        gv->accept(this);
        out << gv->varName << ": .quad 0\n";
    }
    
    // Sección .text
    out << ".text\n";
    
    // Funciones
    for (auto fun : prog->functions) {
        fun->accept(this);
    }
    
    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}
