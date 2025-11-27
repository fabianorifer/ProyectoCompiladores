#include "visitor.h"
#include <iostream>
#include <stdexcept>

using namespace std;

static BaseType inferredI64(I64_TYPE);
static BaseType inferredF64(F64_TYPE);

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
    : out(output), stackOffset(-8), labelCounter(0), inFunction(false), lastExprType(nullptr), lastExprIsFloat(false), usesFloats(false) {}

int GenCodeVisitor::generar(Program* program) {
    return program->accept(this);
}


bool GenCodeVisitor::isFloatType(Type* type) {
    if (!type) return false;
    TypeKindVisitor visitor;
    type->accept(&visitor);
    return visitor.isBaseType && (visitor.baseKind == F32_TYPE || visitor.baseKind == F64_TYPE);
}

bool GenCodeVisitor::isPointerType(Type* type) {
    if (!type) return false;
    TypeKindVisitor visitor;
    type->accept(&visitor);
    return visitor.isPointerType;
}

BaseTypeKind GenCodeVisitor::getBaseTypeKind(Type* type) {
    TypeKindVisitor visitor;
    type->accept(&visitor);
    return visitor.baseKind;
}

Type* GenCodeVisitor::getPointeeType(Type* type) {
    TypeKindVisitor visitor;
    type->accept(&visitor);
    return visitor.pointeeType;
}

// ============================================
// Tipos
// ============================================

int GenCodeVisitor::visit(BaseType* type) {

    return 0;
}

int GenCodeVisitor::visit(PointerType* type) {

    return 0;
}

// ============================================
// Expresiones
// ============================================

int GenCodeVisitor::visit(NumberExp* exp) {
    out << "  movq $" << exp->value << ", %rax\n";
    lastExprIsFloat = false;
    return 0;
}

int GenCodeVisitor::visit(FloatExp* exp) {
    usesFloats = true;  // Marcar que se usan flotantes
    // Generar float literal en .data section
    int floatLabel = labelCounter++;
    out << "  # FloatExp " << exp->value << "\n";
    

    out << ".section .rodata\n";
    out << ".LC" << floatLabel << ":\n";
    out << "  .double " << exp->value << "\n";
    out << ".text\n";
    out << "  movsd .LC" << floatLabel << "(%rip), %xmm0\n";
    
    lastExprIsFloat = true;
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << "  movq $" << (exp->value ? 1 : 0) << ", %rax\n";
    lastExprIsFloat = false;
    return 0;
}

int GenCodeVisitor::visit(StringExp* exp) {

    out << "  # StringExp \"" << exp->value << "\" (not implemented)\n";
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {

    bool isFloat = false;
    if (varTypes.count(exp->id)) {
        isFloat = isFloatType(varTypes[exp->id]);
    }
    
    if (globalVars.count(exp->id)) {
        if (isFloat) {
            out << "  movsd " << exp->id << "(%rip), %xmm0\n";
            lastExprIsFloat = true;
        } else {
            out << "  movq " << exp->id << "(%rip), %rax\n";
            lastExprIsFloat = false;
        }
    } else if (localVars.count(exp->id)) {
        if (isFloat) {
            out << "  movsd " << localVars[exp->id] << "(%rbp), %xmm0\n";
            lastExprIsFloat = true;
        } else {
            out << "  movq " << localVars[exp->id] << "(%rbp), %rax\n";
            lastExprIsFloat = false;
        }
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
    // Caso 1: NumberExp 
    if (exp->isConstant()) {
        result = exp->getConstValue();
        return true;
    }
    
    // Caso 2: BinaryExp 
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

    long long constResult;
    if (tryEvalConst(exp, constResult)) {
        out << "  movq $" << constResult << ", %rax\n";
        lastExprIsFloat = false;
        return 0;
    }
    
    // Evaluar operando izquierdo
    exp->left->accept(this);
    bool leftIsFloat = lastExprIsFloat;
    
    // Guardar resultado izquierdo
    if (leftIsFloat) {
        
        out << "  movsd %xmm0, %xmm2\n";
    } else {
        
        out << "  pushq %rax\n";
    }
    
    // Evaluar operando derecho
    exp->right->accept(this);
    bool rightIsFloat = lastExprIsFloat;
    
    // Determinar si el resultado es float
    bool resultIsFloat = leftIsFloat || rightIsFloat;
    
    if (resultIsFloat) {
       
      
        if (rightIsFloat && !leftIsFloat) {
          
            out << "  movsd %xmm0, %xmm1\n";  
            out << "  popq %rax\n";             
            out << "  cvtsi2sd %rax, %xmm0\n"; 
            
        } else if (!rightIsFloat && leftIsFloat) {
            
            out << "  cvtsi2sd %rax, %xmm1\n"; 
            out << "  movsd %xmm2, %xmm0\n";   
           
        } else {
           
            out << "  movsd %xmm0, %xmm1\n";   
            out << "  movsd %xmm2, %xmm0\n";   
    
        }
        
   
        switch (exp->op) {
            case PLUS_OP:  out << "  addsd %xmm1, %xmm0\n"; break;
            case MINUS_OP: out << "  subsd %xmm1, %xmm0\n"; break;
            case MUL_OP:   out << "  mulsd %xmm1, %xmm0\n"; break;
            case DIV_OP:   out << "  divsd %xmm1, %xmm0\n"; break;
                
  
            case LT_OP:
            case LE_OP:
            case GT_OP:
            case GE_OP:
            case EQ_OP:
            case NE_OP:
                out << "  ucomisd %xmm1, %xmm0\n";
                out << "  movl $0, %eax\n";
                switch (exp->op) {
                    case LT_OP: out << "  setb %al\n"; break;
                    case LE_OP: out << "  setbe %al\n"; break;
                    case GT_OP: out << "  seta %al\n"; break;
                    case GE_OP: out << "  setae %al\n"; break;
                    case EQ_OP: out << "  sete %al\n"; break;
                    case NE_OP: out << "  setne %al\n"; break;
                    default: break;
                }
                lastExprIsFloat = false;  
                return 0;
                
            default:
                out << "  # Operación no soportada para floats\n";
                break;
        }
        lastExprIsFloat = true;
        
    } else {
        // ===== OPERACIONES CON ENTEROS =====
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
        lastExprIsFloat = false;
    }
    return 0;
}

int GenCodeVisitor::visit(UnaryExp* exp) {
    switch (exp->op) {
        case NOT_OP:
            exp->operand->accept(this);
            out << "  cmpq $0, %rax\n";
            out << "  movl $0, %eax\n";
            out << "  sete %al\n";
            lastExprIsFloat = false;
            break;
            
        case NEG_OP:
            exp->operand->accept(this);
            if (lastExprIsFloat) {
             
                out << "  # Negar float/double\n";
                out << "  movq $0x8000000000000000, %rax\n";
                out << "  movq %rax, %xmm1\n";
                out << "  xorpd %xmm1, %xmm0\n";
            } else {
                out << "  negq %rax\n";
            }
            break;
            
        case DEREF_OP: {
        
            exp->operand->accept(this);  // Dirección en %rax
            
            // Determinar tipo apuntado desde varTypes
            IdExp* idOp = exp->operand->asIdExp();
            if (idOp && varTypes.count(idOp->id)) {
                Type* varType = varTypes[idOp->id];
                if (isPointerType(varType)) {
                    Type* pointeeType = getPointeeType(varType);
                    if (pointeeType) {
                        TypeKindVisitor pkVisitor;
                        pointeeType->accept(&pkVisitor);
                        if (pkVisitor.isBaseType) {
                            switch (pkVisitor.baseKind) {
                                case I32_TYPE:
                                    out << "  movl (%rax), %eax\n";
                                    lastExprIsFloat = false;
                                    break;
                                case I64_TYPE:
                                    out << "  movq (%rax), %rax\n";
                                    lastExprIsFloat = false;
                                    break;
                                case F32_TYPE:
                                    out << "  movss (%rax), %xmm0\n";
                                    lastExprIsFloat = true;
                                    break;
                                case F64_TYPE:
                                    out << "  movsd (%rax), %xmm0\n";
                                    lastExprIsFloat = true;
                                    break;
                                case BOOL_TYPE:
                                    out << "  movzbl (%rax), %eax\n";
                                    lastExprIsFloat = false;
                                    break;
                            }
                        } else {
                           
                            out << "  movq (%rax), %rax\n";
                            lastExprIsFloat = false;
                        }
                    } else {
                        
                        out << "  movq (%rax), %rax\n";
                        lastExprIsFloat = false;
                    }
                } else {
                
                    out << "  movq (%rax), %rax\n";
                    lastExprIsFloat = false;
                }
            } else {
               
                out << "  movq (%rax), %rax\n";
                lastExprIsFloat = false;
            }
            break;
        }
            
        case REF_OP:
        case REF_MUT_OP: {
            // Obtener dirección de variable
            IdExp* idOp = exp->operand->asIdExp();
            if (idOp) {
                string varName = idOp->id;
                if (localVars.count(varName)) {
                    int offset = localVars[varName];
                    out << "  leaq " << offset << "(%rbp), %rax\n";
                } else if (globalVars.count(varName)) {
                    out << "  leaq " << varName << "(%rip), %rax\n";
                } else {
                    out << "  # Error: variable no encontrada: " << varName << "\n";
                }
            } else {
               
                exp->operand->accept(this);
            }
            lastExprIsFloat = false;  
            break;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(FunCallExp* exp) {
    vector<string> intArgRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    vector<string> floatArgRegs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};
    size_t intArgIndex = 0;
    size_t floatArgIndex = 0;
    int stackArgBytes = 0;

    for (Exp* arg : exp->args) {
        arg->accept(this);

        if (lastExprIsFloat) {
            if (floatArgIndex < floatArgRegs.size()) {
                out << "  movsd %xmm0, " << floatArgRegs[floatArgIndex++] << "\n";
            } else {
                out << "  subq $8, %rsp\n";
                out << "  movsd %xmm0, (%rsp)\n";
                stackArgBytes += 8;
            }
        } else {
            if (intArgIndex < intArgRegs.size()) {
                out << "  movq %rax, " << intArgRegs[intArgIndex++] << "\n";
            } else {
                out << "  pushq %rax\n";
                stackArgBytes += 8;
            }
        }
    }

    out << "  movl $0, %eax\n";
    out << "  call " << exp->funName << "\n";

    if (stackArgBytes > 0) {
        out << "  addq $" << stackArgBytes << ", %rsp\n";
    }
    return 0;
}

int GenCodeVisitor::visit(CastExp* exp) {
 
    exp->expr->accept(this);
    bool sourceIsFloat = lastExprIsFloat;
    
 
    bool sourceIsPointer = false;
    
    // Caso 1: UnaryExp con operadores de puntero
    UnaryExp* unarySource = exp->expr->asUnaryExp();
    if (unarySource && (unarySource->op == REF_OP || unarySource->op == REF_MUT_OP || unarySource->op == DEREF_OP)) {
        sourceIsPointer = true;
    }
    
    // Caso 2: IdExp cuyo tipo es PointerType
    IdExp* idSource = exp->expr->asIdExp();
    if (idSource && varTypes.count(idSource->id)) {
        if (isPointerType(varTypes[idSource->id])) {
            sourceIsPointer = true;
        }
    }
    
    // Determinar tipo de destino
    TypeKindVisitor targetVisitor;
    exp->targetType->accept(&targetVisitor);
    bool targetIsPointer = targetVisitor.isPointerType;
    bool targetIsBaseType = targetVisitor.isBaseType;
    BaseTypeKind targetKind = targetVisitor.baseKind;
    
    if (targetIsPointer) {
        // Cast a puntero
        if (sourceIsFloat) {
            
            out << "  # ERROR: cast de float a puntero no permitido\n";
            lastExprIsFloat = false;
            return 0;
        }
        
        out << "  # puntero/i64 → puntero (no-op, ya en %rax)\n";
        lastExprIsFloat = false;
        return 0;
    }
    
    if (!targetIsBaseType) {
        out << "  # Cast to unknown type\n";
        return 0;
    }
    
    // ====================
    // Puntero → i64
    // ====================
    if (sourceIsPointer && targetKind == I64_TYPE) {
        // Puntero ya está en %rax como i64
        out << "  # puntero → i64 (no-op, ya en %rax)\n";
        lastExprIsFloat = false;
        return 0;
    }
    
    // ====================
    // CASO 1: Entero → Entero
    // ====================
    if (!sourceIsFloat && !sourceIsPointer && (targetKind == I32_TYPE || targetKind == I64_TYPE)) {
        // Asumimos que la fuente es i32 o i64
        // i32 → i64: extensión de signo
        if (targetKind == I64_TYPE) {
            out << "  movsxd %eax, %rax  # i32 → i64 (extensión de signo)\n";
        } else {
            // i64 → i32: truncado (solo usar parte baja)
            // %eax ya tiene los 32 bits bajos, no hacer nada
            out << "  # i64 → i32 (truncado implícito en %eax)\n";
        }
        lastExprIsFloat = false;
        return 0;
    }
    
    // ====================
    // CASO 2: Entero → Float
    // ====================
    if (!sourceIsFloat && (targetKind == F32_TYPE || targetKind == F64_TYPE)) {
        if (targetKind == F64_TYPE) {
            out << "  cvtsi2sd %rax, %xmm0  # i64 → f64\n";
        } else {
            out << "  cvtsi2ss %rax, %xmm0  # i64 → f32\n";
        }
        lastExprIsFloat = true;
        return 0;
    }
    
    // ====================
    // CASO 3: Float → Entero (trunca hacia cero)
    // ====================
    if (sourceIsFloat && (targetKind == I32_TYPE || targetKind == I64_TYPE)) {
        if (targetKind == I32_TYPE) {
            out << "  cvttsd2si %xmm0, %eax  # f64 → i32 (trunca)\n";
        } else {
            out << "  cvttsd2si %xmm0, %rax  # f64 → i64 (trunca)\n";
        }
        lastExprIsFloat = false;
        return 0;
    }
    
    // ====================
    // CASO 4: Float → Float
    // ====================
    if (sourceIsFloat && (targetKind == F32_TYPE || targetKind == F64_TYPE)) {
        // Asumimos que la fuente es f64 (siempre usamos double en XMM)
        if (targetKind == F32_TYPE) {
            out << "  cvtsd2ss %xmm0, %xmm0  # f64 → f32\n";
        } else {
            // f64 → f64 o f32 → f64: no hacer nada o convertir
            out << "  # f64 → f64 (sin cambio)\n";
        }
        lastExprIsFloat = true;
        return 0;
    }
    
    // ====================
    // CASO 5: Bool ↔ Entero
    // ====================
    if (targetKind == BOOL_TYPE) {
        if (sourceIsFloat) {
            // Float → Bool: comparar con 0.0
            out << "  xorpd %xmm1, %xmm1  # Crear 0.0\n";
            out << "  ucomisd %xmm1, %xmm0\n";
            out << "  setne %al\n";
            out << "  movzbl %al, %eax\n";
            lastExprIsFloat = false;
        } else {
            // Entero → Bool: comparar con 0
            out << "  testq %rax, %rax\n";
            out << "  setne %al\n";
            out << "  movzbl %al, %eax\n";
            lastExprIsFloat = false;
        }
        return 0;
    }
    
    if (!sourceIsFloat && targetKind == BOOL_TYPE) {
        // Ya manejado arriba
        return 0;
    }
    
    out << "  # Cast no implementado\n";
    return 0;
}

int GenCodeVisitor::visit(IfExp* exp) {
    // Optimización: verificar si la condición es constante
    long long constCond;
    if (tryEvalConst(exp->condition, constCond)) {
        // Condición constante: solo generar la rama tomada
        if (constCond != 0) {
            // Condición true: solo ejecutar then
            exp->thenBlock->accept(this);
        } else {
            // Condición false: solo ejecutar else (si existe)
            if (exp->elseBlock) {
                exp->elseBlock->accept(this);
            }
        }
        return 0;
    }
    
   
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
        if (stm->varType) {
            varTypes[stm->varName] = stm->varType;
        }

        if (stm->initializer) {
            stm->initializer->accept(this);

            bool declaredAsFloat = false;
            if (stm->varType) {
                declaredAsFloat = isFloatType(stm->varType);
            }

            bool initializerIsFloat = lastExprIsFloat;
            if (initializerIsFloat || declaredAsFloat) {
                out << "  movsd %xmm0, " << localVars[stm->varName] << "(%rbp)\n";
                if (!stm->varType) {
                    varTypes[stm->varName] = &inferredF64;
                }
            } else {
                out << "  movq %rax, " << localVars[stm->varName] << "(%rbp)\n";
                if (!stm->varType) {
                    varTypes[stm->varName] = &inferredI64;
                }
            }
        }
    }
    return 0;
}

int GenCodeVisitor::visit(AssignStm* stm) {

    UnaryExp* unaryLhs = stm->lhs->asUnaryExp();
    if (unaryLhs && unaryLhs->op == DEREF_OP) {
        
    
        stm->rhs->accept(this);
        bool rhsIsFloat = lastExprIsFloat;
  
        if (rhsIsFloat) {
            out << "  subq $8, %rsp\n";
            out << "  movsd %xmm0, (%rsp)\n"; 
        } else {
            out << "  pushq %rax\n";
        }
        
        unaryLhs->operand->accept(this);
        out << "  movq %rax, %rcx\n";  

       
        if (rhsIsFloat) {
            out << "  movsd (%rsp), %xmm0\n";
            out << "  addq $8, %rsp\n";
            // Determinar si es f32 o f64
            IdExp* ptrId = unaryLhs->operand->asIdExp();
            bool isF32 = false;
            if (ptrId && varTypes.count(ptrId->id)) {
                Type* ptrType = varTypes[ptrId->id];
                if (isPointerType(ptrType)) {
                    Type* pointee = getPointeeType(ptrType);
                    if (pointee) {
                        TypeKindVisitor pkVisitor;
                        pointee->accept(&pkVisitor);
                        if (pkVisitor.isBaseType && pkVisitor.baseKind == F32_TYPE) {
                            isF32 = true;
                        }
                    }
                }
            }
            if (isF32) {
                out << "  movss %xmm0, (%rcx)\n";
            } else {
                out << "  movsd %xmm0, (%rcx)\n";
            }
        } else {
            out << "  popq %rax\n";
            // Determinar tamaño según tipo
            IdExp* ptrId = unaryLhs->operand->asIdExp();
            bool isI32 = false;
            if (ptrId && varTypes.count(ptrId->id)) {
                Type* ptrType = varTypes[ptrId->id];
                if (isPointerType(ptrType)) {
                    Type* pointee = getPointeeType(ptrType);
                    if (pointee) {
                        TypeKindVisitor pkVisitor;
                        pointee->accept(&pkVisitor);
                        if (pkVisitor.isBaseType && (pkVisitor.baseKind == I32_TYPE || pkVisitor.baseKind == BOOL_TYPE)) {
                            isI32 = true;
                        }
                    }
                }
            }
            if (isI32) {
                out << "  movl %eax, (%rcx)\n";
            } else {
                out << "  movq %rax, (%rcx)\n";
            }
        }
        return 0;
    }
    
    stm->rhs->accept(this);
    bool rhsIsFloat = lastExprIsFloat;

    IdExp* idLhs = stm->lhs->asIdExp();
    if (idLhs) {
        bool wasKnown = false;
        bool knownAsFloat = false;
        auto it = varTypes.find(idLhs->id);
        if (it != varTypes.end()) {
            wasKnown = true;
            knownAsFloat = isFloatType(it->second);
        }

        bool treatAsFloat = knownAsFloat || rhsIsFloat;
        if (rhsIsFloat && !knownAsFloat) {
            varTypes[idLhs->id] = &inferredF64;
            knownAsFloat = true;
            treatAsFloat = true;
        } else if (!rhsIsFloat && !wasKnown) {
            varTypes[idLhs->id] = &inferredI64;
        }

        if (globalVars.count(idLhs->id)) {
            if (stm->op != ASSIGN) {
                if (treatAsFloat) {
                    out << "  movsd %xmm0, %xmm1\n";
                    out << "  movsd " << idLhs->id << "(%rip), %xmm0\n";
                    switch (stm->op) {
                        case PLUS_ASSIGN:  out << "  addsd %xmm1, %xmm0\n"; break;
                        case MINUS_ASSIGN: out << "  subsd %xmm1, %xmm0\n"; break;
                        case MUL_ASSIGN:   out << "  mulsd %xmm1, %xmm0\n"; break;
                        case DIV_ASSIGN:   out << "  divsd %xmm1, %xmm0\n"; break;
                        default: break;
                    }
                    out << "  movsd %xmm0, " << idLhs->id << "(%rip)\n";
                } else {
                    out << "  movq %rax, %rcx\n";
                    out << "  movq " << idLhs->id << "(%rip), %rax\n";
                    switch (stm->op) {
                        case PLUS_ASSIGN:  out << "  addq %rcx, %rax\n"; break;
                        case MINUS_ASSIGN: out << "  subq %rcx, %rax\n"; break;
                        case MUL_ASSIGN:   out << "  imulq %rcx, %rax\n"; break;
                        case DIV_ASSIGN:   out << "  cqto\\n  idivq %rcx\n"; break;
                        case MOD_ASSIGN:   out << "  cqto\\n  idivq %rcx\\n  movq %rdx, %rax\n"; break;
                        default: break;
                    }
                    out << "  movq %rax, " << idLhs->id << "(%rip)\n";
                }
            } else {
                if (treatAsFloat) {
                    out << "  movsd %xmm0, " << idLhs->id << "(%rip)\n";
                } else {
                    out << "  movq %rax, " << idLhs->id << "(%rip)\n";
                }
            }
        } else if (localVars.count(idLhs->id)) {
            if (stm->op != ASSIGN) {
                if (treatAsFloat) {
                    out << "  movsd %xmm0, %xmm1\n";
                    out << "  movsd " << localVars[idLhs->id] << "(%rbp), %xmm0\n";
                    switch (stm->op) {
                        case PLUS_ASSIGN:  out << "  addsd %xmm1, %xmm0\n"; break;
                        case MINUS_ASSIGN: out << "  subsd %xmm1, %xmm0\n"; break;
                        case MUL_ASSIGN:   out << "  mulsd %xmm1, %xmm0\n"; break;
                        case DIV_ASSIGN:   out << "  divsd %xmm1, %xmm0\n"; break;
                        default: break;
                    }
                    out << "  movsd %xmm0, " << localVars[idLhs->id] << "(%rbp)\n";
                } else {
                    out << "  movq %rax, %rcx\n";
                    out << "  movq " << localVars[idLhs->id] << "(%rbp), %rax\n";
                    switch (stm->op) {
                        case PLUS_ASSIGN:  out << "  addq %rcx, %rax\n"; break;
                        case MINUS_ASSIGN: out << "  subq %rcx, %rax\n"; break;
                        case MUL_ASSIGN:   out << "  imulq %rcx, %rax\n"; break;
                        case DIV_ASSIGN:   out << "  cqto\\n  idivq %rcx\n"; break;
                        case MOD_ASSIGN:   out << "  cqto\\n  idivq %rcx\\n  movq %rdx, %rax\n"; break;
                        default: break;
                    }
                    out << "  movq %rax, " << localVars[idLhs->id] << "(%rbp)\n";
                }
            } else {
                if (treatAsFloat) {
                    out << "  movsd %xmm0, " << localVars[idLhs->id] << "(%rbp)\n";
                } else {
                    out << "  movq %rax, " << localVars[idLhs->id] << "(%rbp)\n";
                }
            }
        }
    }
    return 0;
}

int GenCodeVisitor::visit(PrintlnStm* stm) {
    if (!stm->args.empty()) {
        Exp* optimized = stm->args[0]->optimize();
        optimized->accept(this);
        
        if (lastExprIsFloat) {
            out << "  leaq float_fmt(%rip), %rdi\n";
            out << "  movl $1, %eax  # 1 argumento XMM\n";
            out << "  call printf@PLT\n";
        } else {
            // Entero: formato "%ld\n"
            out << "  movq %rax, %rsi\n";
            out << "  leaq print_fmt(%rip), %rdi\n";
            out << "  movl $0, %eax\n";
            out << "  call printf@PLT\n";
        }
    }
    return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    // Optimización: verificar si la condición es constante
    long long constCond;
    if (tryEvalConst(stm->condition, constCond)) {
        // Condición constante: solo generar la rama tomada
        if (constCond != 0) {
            // Condición true: solo ejecutar then
            stm->thenBlock->accept(this);
        } else {
            // Condición false: solo ejecutar else (si existe)
            if (stm->elseBlock) {
                stm->elseBlock->accept(this);
            }
        }
        return 0;
    }
    
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

// ============================================
// Pre-pasada para detectar flotantes
// ============================================

void GenCodeVisitor::detectFloats(Program* prog) {
    // Verificar variables globales
    for (auto gv : prog->globalVars) {
        if (gv->varType && isFloatType(gv->varType)) {
            usesFloats = true;
            return;
        }
    }
    
    // Verificar funciones
    for (auto fun : prog->functions) {
        // Verificar parámetros
        for (auto param : fun->params) {
            if (param->type && isFloatType(param->type)) {
                usesFloats = true;
                return;
            }
        }
        
        // Verificar tipo de retorno
        if (fun->returnType && isFloatType(fun->returnType)) {
            usesFloats = true;
            return;
        }
        
        // Verificar cuerpo
        if (fun->body) {
            detectFloatsInBlock(fun->body);
            if (usesFloats) return;
        }
    }
}

void GenCodeVisitor::detectFloatsInBlock(Block* block) {
    for (auto stmt : block->stmts) {
        detectFloatsInStmt(stmt);
        if (usesFloats) return;
    }
}

void GenCodeVisitor::detectFloatsInStmt(Stm* stmt) {
    if (usesFloats) return;
    
    VarDeclStm* varDecl = stmt->asVarDeclStm();
    if (varDecl) {
        if (varDecl->varType && isFloatType(varDecl->varType)) {
            usesFloats = true;
            return;
        }
        if (varDecl->initializer) {
            detectFloatsInExp(varDecl->initializer);
        }
        return;
    }
    
    Block* block = stmt->asBlock();
    if (block) {
        detectFloatsInBlock(block);
        return;
    }
    
    IfStm* ifStm = stmt->asIfStm();
    if (ifStm) {
        if (ifStm->condition) detectFloatsInExp(ifStm->condition);
        if (usesFloats) return;
        detectFloatsInBlock(ifStm->thenBlock);
        if (usesFloats) return;
        if (ifStm->elseBlock) detectFloatsInBlock(ifStm->elseBlock);
        return;
    }
    
    WhileStm* whileStm = stmt->asWhileStm();
    if (whileStm) {
        if (whileStm->condition) detectFloatsInExp(whileStm->condition);
        if (usesFloats) return;
        detectFloatsInBlock(whileStm->body);
        return;
    }
    
    ForRangeStm* forStm = stmt->asForRangeStm();
    if (forStm) {
        if (forStm->rangeStart) detectFloatsInExp(forStm->rangeStart);
        if (usesFloats) return;
        if (forStm->rangeEnd) detectFloatsInExp(forStm->rangeEnd);
        if (usesFloats) return;
        detectFloatsInBlock(forStm->body);
        return;
    }
    
    // AssignStm, PrintlnStm, ReturnStm, ExprStm - verificar expresiones
    ExprStm* exprStm = stmt->asExprStm();
    if (exprStm && exprStm->expr) {
        detectFloatsInExp(exprStm->expr);
    }
}

void GenCodeVisitor::detectFloatsInExp(Exp* exp) {
    if (usesFloats || !exp) return;
    
    // Verificar si es FloatExp
    if (dynamic_cast<FloatExp*>(exp)) {
        usesFloats = true;
        return;
    }
    
    // Verificar BinaryExp
    BinaryExp* binExp = exp->asBinaryExp();
    if (binExp) {
        detectFloatsInExp(binExp->left);
        if (usesFloats) return;
        detectFloatsInExp(binExp->right);
        return;
    }

}

int GenCodeVisitor::visit(GlobalVarDecl* decl) {
    globalVars[decl->varName] = true;
    if (decl->varType) {
        varTypes[decl->varName] = decl->varType;
    }
    return 0;
}

int GenCodeVisitor::visit(FunDecl* decl) {
    inFunction = true;
    currentFunction = decl->name;
    localVars.clear();
    stackOffset = -8;
    
    vector<string> intArgRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    vector<string> floatArgRegs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};
    size_t intArgIndex = 0;
    size_t floatArgIndex = 0;
    
    out << ".globl " << decl->name << "\n";
    out << decl->name << ":\n";
    out << "  pushq %rbp\n";
    out << "  movq %rsp, %rbp\n";
    
    // Parámetros
    for (auto* param : decl->params) {
        localVars[param->name] = stackOffset;
        if (param->type) {
            varTypes[param->name] = param->type;
        }
        stackOffset -= 8;
    }
    
    // Pre-pase: contar todas las variables locales
    if (decl->body) {
        countLocalVars(decl->body);
    }
    
    // Reservar espacio en stack ANTES de mover parámetros
    int stackSize = -stackOffset - 8;
    // Alinear a 16 bytes
    if (stackSize % 16 != 0) {
        stackSize += 16 - (stackSize % 16);
    }
    if (stackSize > 0) {
        out << "  subq $" << stackSize << ", %rsp\n";
    }
    
 
    for (auto* param : decl->params) {
        bool isFloatParam = false;
        if (param->type) {
            isFloatParam = isFloatType(param->type);
        }

        if (isFloatParam) {
            if (floatArgIndex < floatArgRegs.size()) {
                out << "  movsd " << floatArgRegs[floatArgIndex++] << ", " << localVars[param->name] << "(%rbp)\n";
            } else {
                out << "  # TODO: argumentos float adicionales no soportados aún\n";
            }
        } else {
            if (intArgIndex < intArgRegs.size()) {
                out << "  movq " << intArgRegs[intArgIndex++] << ", " << localVars[param->name] << "(%rbp)\n";
            } else {
                out << "  # TODO: argumentos enteros adicionales no soportados aún\n";
            }
        }
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
    detectFloats(prog);
    
    // Sección .data
    out << ".data\n";
    out << "print_fmt: .string \"%ld \\n\"\n";
    if (usesFloats) {
        out << "float_fmt: .string \"%f \\n\"\n";
    }
    
    // Variables globales
    for (auto gv : prog->globalVars) {
        gv->accept(this);
 
        bool isFloat = false;
        if (gv->varType) {
            isFloat = isFloatType(gv->varType);
        }
        
        if (isFloat) {
            out << gv->varName << ": .double 0.0\n";
        } else {
            out << gv->varName << ": .quad 0\n";
        }
    }
    
    out << ".text\n";
    
    // Funciones
    for (auto fun : prog->functions) {
        fun->accept(this);
    }
    
    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}
