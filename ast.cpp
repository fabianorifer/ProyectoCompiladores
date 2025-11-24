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


