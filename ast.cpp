#include "ast.h"
#include <iostream>

using namespace std;

// ============================================
// TIPOS
// ============================================

string BaseType::toString() {
    switch (kind) {
        case I32_TYPE:  return "i32";
        case I64_TYPE:  return "i64";
        case F32_TYPE:  return "f32";
        case F64_TYPE:  return "f64";
        case BOOL_TYPE: return "bool";
        default:        return "unknown";
    }
}

string PointerType::toString() {
    string prefix = isMutable ? "*mut " : "*const ";
    return prefix + pointeeType->toString();
}

// ============================================
// OPERADORES
// ============================================

string Exp::binopToString(BinaryOp op) {
    switch (op) {
        case PLUS_OP:   return "+";
        case MINUS_OP:  return "-";
        case MUL_OP:    return "*";
        case DIV_OP:    return "/";
        case MOD_OP:    return "%";
        case POW_OP:    return "**";
        case LT_OP:     return "<";
        case LE_OP:     return "<=";
        case GT_OP:     return ">";
        case GE_OP:     return ">=";
        case EQ_OP:     return "==";
        case NE_OP:     return "!=";
        case AND_OP:    return "&&";
        case OR_OP:     return "||";
        default:        return "?";
    }
}

string Exp::unopToString(UnaryOp op) {
    switch (op) {
        case NOT_OP:     return "!";
        case NEG_OP:     return "-";
        case DEREF_OP:   return "*";
        case REF_OP:     return "&";
        case REF_MUT_OP: return "&mut";
        default:         return "?";
    }
}

// ============================================
// DESTRUCTORES IfExp
// ============================================

IfExp::~IfExp() {
    delete condition;
    delete thenBlock;
    if (elseBlock) delete elseBlock;
}

// ============================================
// OPTIMIZACION
// ============================================

Exp* BinaryExp::optimize() {
    // Primero optimizar operandos recursivamente
    Exp* newLeft = left->optimize();
    Exp* newRight = right->optimize();
    
    // Si ambos operandos son constantes, los evaluamos en tiempo de compilación
    if (newLeft->isConstant() && newRight->isConstant()) {
        long long lval = newLeft->getConstValue();
        long long rval = newRight->getConstValue();
        long long result = 0;
        bool canOptimize = true;
        
        switch (op) {
            case PLUS_OP:  result = lval + rval; break;
            case MINUS_OP: result = lval - rval; break;
            case MUL_OP:   result = lval * rval; break;
            case DIV_OP:   if (rval != 0) result = lval / rval; else canOptimize = false; break;
            case MOD_OP:   if (rval != 0) result = lval % rval; else canOptimize = false; break;
            case LT_OP:    result = lval < rval ? 1 : 0; break;
            case LE_OP:    result = lval <= rval ? 1 : 0; break;
            case GT_OP:    result = lval > rval ? 1 : 0; break;
            case GE_OP:    result = lval >= rval ? 1 : 0; break;
            case EQ_OP:    result = lval == rval ? 1 : 0; break;
            case NE_OP:    result = lval != rval ? 1 : 0; break;
            case AND_OP:   result = (lval && rval) ? 1 : 0; break;
            case OR_OP:    result = (lval || rval) ? 1 : 0; break;
            default: canOptimize = false; break;
        }
        
        if (canOptimize) {
            // Creamos NumberExp con el resultado
            return new NumberExp(result);
        }
    }
    
    // Si hubo cambios en los operandos, los cambiamos
    if (newLeft != left || newRight != right) {
        left = newLeft;
        right = newRight;
    }
    
    return this;  
}


