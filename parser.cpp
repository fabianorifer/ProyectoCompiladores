#include <iostream>
#include <stdexcept>
#include "parser.h"
#include "token.h"

using namespace std;

// ============================================
// Constructor y métodos auxiliares
// ============================================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico en el scanner");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            throw runtime_error("Error léxico: " + current->text);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

// ============================================
// Parsing de Programa
// ============================================

Program* Parser::parseProgram() {
    Program* prog = new Program();
    
    while (!isAtEnd()) {
        if (check(Token::FN)) {
            prog->functions.push_back(parseFunDecl());
        } else if (check(Token::LET)) {
            prog->globalVars.push_back(parseGlobalVarDecl());
        } else {
            throw runtime_error("Se esperaba 'fn' o 'let' al nivel superior");
        }
    }
    
    return prog;
}

// ============================================
// Parsing de declaraciones globales
// ============================================

GlobalVarDecl* Parser::parseGlobalVarDecl() {
    if (!match(Token::LET)) {
        throw runtime_error("Se esperaba 'let'");
    }
    
    bool isMut = match(Token::MUT);
    
    if (!match(Token::ID)) {
        throw runtime_error("Se esperaba identificador después de 'let'");
    }
    string varName = previous->text;
    
    Type* varType = nullptr;
    Exp* initializer = nullptr;
    
    // TypeAnnotation: ':' Type
    if (match(Token::COLON)) {
        varType = parseType();
        
        // InitializerOpt: '=' Expr | ε
        if (match(Token::ASSIGN)) {
            initializer = parseExpr();
        }
    } else if (match(Token::ASSIGN)) {
        // Inferencia: requiere inicializador
        initializer = parseExpr();
    } else {
        throw runtime_error("Variable global requiere tipo o inicializador");
    }
    
    if (!match(Token::SEMICOL)) {
        throw runtime_error("Se esperaba ';' después de declaración global");
    }
    
    return new GlobalVarDecl(isMut, varName, varType, initializer);
}

FunDecl* Parser::parseFunDecl() {
    if (!match(Token::FN)) {
        throw runtime_error("Se esperaba 'fn'");
    }
    
    if (!match(Token::ID)) {
        throw runtime_error("Se esperaba nombre de función");
    }
    string funName = previous->text;
    FunDecl* fun = new FunDecl(funName);
    
    if (!match(Token::LPAREN)) {
        throw runtime_error("Se esperaba '(' después del nombre de función");
    }
    
    // ParamListOpt
    if (!check(Token::RPAREN)) {
        fun->params.push_back(parseParam());
        while (match(Token::COMA)) {
            fun->params.push_back(parseParam());
        }
    }
    
    if (!match(Token::RPAREN)) {
        throw runtime_error("Se esperaba ')' después de parámetros");
    }
    
    // ReturnTypeOpt: '->' Type
    if (match(Token::ARROW)) {
        fun->returnType = parseType();
    }
    
    fun->body = parseBlock();
    
    return fun;
}

Param* Parser::parseParam() {
    if (!match(Token::ID)) {
        throw runtime_error("Se esperaba nombre de parámetro");
    }
    string paramName = previous->text;
    
    if (!match(Token::COLON)) {
        throw runtime_error("Se esperaba ':' después del nombre del parámetro");
    }
    
    Type* paramType = parseType();
    
    return new Param(paramName, paramType);
}

// ============================================
// Parsing de tipos
// ============================================

Type* Parser::parseType() {
    // PointerType | BaseType
    if (check(Token::STAR_MUT) || check(Token::STAR_CONST)) {
        return parsePointerType();
    }
    if (check(Token::MUL)) {
        return parsePointerType();
    }
    return parseBaseType();
}

Type* Parser::parseBaseType() {
    if (match(Token::I32)) {
        return new BaseType(I32_TYPE);
    } else if (match(Token::I64)) {
        return new BaseType(I64_TYPE);
    } else if (match(Token::F32)) {
        return new BaseType(F32_TYPE);
    } else if (match(Token::F64)) {
        return new BaseType(F64_TYPE);
    } else if (match(Token::BOOL)) {
        return new BaseType(BOOL_TYPE);
    } else {
        throw runtime_error("Se esperaba tipo base (i32, i64, f32, f64, bool)");
    }
}

Type* Parser::parsePointerType() {
    // '*mut' Type | '*const' Type
    bool isMut = false;
    
    if (match(Token::STAR_MUT)) {
        isMut = true;
    } else if (match(Token::STAR_CONST)) {
        isMut = false;
    } else if (match(Token::MUL)) {
        // Versión separada: * seguido de mut o const
        if (match(Token::MUT)) {
            isMut = true;
        } else if (!match(Token::ID) || previous->text != "const") {
            throw runtime_error("Se esperaba 'mut' o 'const' después de '*'");
        }
    } else {
        throw runtime_error("Se esperaba '*mut' o '*const' para puntero");
    }
    
    Type* pointeeType = parseType(); // Recursivo
    
    return new PointerType(isMut, pointeeType);
}

// ============================================
// Parsing de statements
// ============================================

Block* Parser::parseBlock() {
    if (!match(Token::LBRACE)) {
        throw runtime_error("Se esperaba '{'");
    }
    
    Block* block = new Block();
    
    while (!check(Token::RBRACE) && !isAtEnd()) {
        // Mirar si es la última expresión (sin ';') antes de '}'
        // Esto sería un return implícito en Rust
        
        // Guardar posición para verificar si viene ';'
        int savedCurrent = current->line;
        
        Stm* stmt = parseStmt();
        block->stmts.push_back(stmt);
    }
    
    if (!match(Token::RBRACE)) {
        throw runtime_error("Se esperaba '}'");
    }
    
    return block;
}

Stm* Parser::parseStmt() {
    // SimpleStmt ';' | IfExpr | WhileStmt | ForRangeStmt | Block | PrintlnStmt ';' | unsafe Block
    
    // unsafe block: simplemente ignoramos 'unsafe' y parseamos el bloque
    if (match(Token::UNSAFE)) {
        if (!check(Token::LBRACE)) {
            throw runtime_error("Se esperaba '{' después de 'unsafe'");
        }
        return parseBlock();
    }
    
    if (check(Token::LBRACE)) {
        return parseBlock();
    }
    
    if (check(Token::IF)) {
        // IfExpr puede ser statement
        Exp* ifExpr = parseIfExpr();
        return new ExprStm(ifExpr);
    }
    
    if (check(Token::WHILE)) {
        return parseWhileStmt();
    }
    
    if (check(Token::FOR)) {
        return parseForRangeStmt();
    }
    
    if (check(Token::PRINTLN)) {
        Stm* s = parsePrintlnStmt();
        if (!match(Token::SEMICOL)) {
            throw runtime_error("Se esperaba ';' después de println!");
        }
        return s;
    }
    
    // SimpleStmt que puede o no tener ';'
    // Si es la última expresión del bloque (antes de '}'), es return implícito
    Stm* s = parseSimpleStmt();
    
    // Verificar si sigue ';' o si es el final del bloque
    if (check(Token::RBRACE)) {
        // Es la última expresión del bloque, return implícito
        // Convertir ExprStm en ReturnStm si aplica
        ExprStm* exprStm = s->asExprStm();
        if (exprStm) {
            ReturnStm* retStm = new ReturnStm(exprStm->expr);
            exprStm->expr = nullptr; // Evitar double delete
            delete exprStm;
            return retStm;
        }
        return s;
    }
    
    if (!match(Token::SEMICOL)) {
        throw runtime_error("Se esperaba ';' después de statement");
    }
    return s;
}

Stm* Parser::parseSimpleStmt() {
    if (check(Token::LET)) {
        return parseVarDecl();
    }
    
    if (check(Token::RETURN)) {
        return parseReturnStmt();
    }
    
    // Podría ser AssignStmt o ExprStmt
    // Necesitamos lookahead para distinguir
    // Simplificación: parseamos expresión y verificamos si sigue '=' o sus variantes
    
    Exp* expr = parseExpr();
    
    // Si sigue un operador de asignación, es AssignStm
    if (check(Token::ASSIGN) || check(Token::PLUS_ASSIGN) || 
        check(Token::MINUS_ASSIGN) || check(Token::MUL_ASSIGN) ||
        check(Token::DIV_ASSIGN) || check(Token::MOD_ASSIGN)) {
        
        AssignOp op = ASSIGN;
        if (match(Token::ASSIGN)) {
            op = ASSIGN;
        } else if (match(Token::PLUS_ASSIGN)) {
            op = PLUS_ASSIGN;
        } else if (match(Token::MINUS_ASSIGN)) {
            op = MINUS_ASSIGN;
        } else if (match(Token::MUL_ASSIGN)) {
            op = MUL_ASSIGN;
        } else if (match(Token::DIV_ASSIGN)) {
            op = DIV_ASSIGN;
        } else if (match(Token::MOD_ASSIGN)) {
            op = MOD_ASSIGN;
        }
        
        Exp* rhs = parseExpr();
        rhs = rhs->optimize();  // Optimizar expresión
        return new AssignStm(expr, op, rhs);
    }
    
    // Es ExprStmt
    return new ExprStm(expr);
}

VarDeclStm* Parser::parseVarDecl() {
    if (!match(Token::LET)) {
        throw runtime_error("Se esperaba 'let'");
    }
    
    bool isMut = match(Token::MUT);
    
    if (!match(Token::ID)) {
        throw runtime_error("Se esperaba identificador");
    }
    string varName = previous->text;
    
    Type* varType = nullptr;
    Exp* initializer = nullptr;
    
    if (match(Token::COLON)) {
        varType = parseType();
        if (match(Token::ASSIGN)) {
            initializer = parseExpr();
            initializer = initializer->optimize();  // Optimizar expresión
        }
    } else if (match(Token::ASSIGN)) {
        initializer = parseExpr();
        initializer = initializer->optimize();  // Optimizar expresión
    } else {
        throw runtime_error("Declaración local requiere tipo o inicializador");
    }
    
    return new VarDeclStm(isMut, varName, varType, initializer);
}

AssignStm* Parser::parseAssignStmt() {
    Exp* lhs = parseExpr();
    
    AssignOp op = ASSIGN;
    if (match(Token::ASSIGN)) {
        op = ASSIGN;
    } else if (match(Token::PLUS_ASSIGN)) {
        op = PLUS_ASSIGN;
    } else if (match(Token::MINUS_ASSIGN)) {
        op = MINUS_ASSIGN;
    } else if (match(Token::MUL_ASSIGN)) {
        op = MUL_ASSIGN;
    } else if (match(Token::DIV_ASSIGN)) {
        op = DIV_ASSIGN;
    } else if (match(Token::MOD_ASSIGN)) {
        op = MOD_ASSIGN;
    } else {
        throw runtime_error("Se esperaba operador de asignación");
    }
    
    Exp* rhs = parseExpr();
    
    return new AssignStm(lhs, op, rhs);
}

ReturnStm* Parser::parseReturnStmt() {
    if (!match(Token::RETURN)) {
        throw runtime_error("Se esperaba 'return'");
    }
    
    Exp* returnValue = nullptr;
    
    // ExprOpt: no parsear si viene ';'
    if (!check(Token::SEMICOL)) {
        returnValue = parseExpr();
    }
    
    return new ReturnStm(returnValue);
}

ExprStm* Parser::parseExprStmt() {
    Exp* expr = parseExpr();
    return new ExprStm(expr);
}

IfStm* Parser::parseIfStmt() {
    if (!match(Token::IF)) {
        throw runtime_error("Se esperaba 'if'");
    }
    
    Exp* condition = parseExpr();
    condition = condition->optimize();  // Optimizar condición
    Block* thenBlock = parseBlock();
    Block* elseBlock = nullptr;
    
    if (match(Token::ELSE)) {
        if (check(Token::IF)) {
            // else if: creamos bloque con IfStm dentro
            elseBlock = new Block();
            elseBlock->stmts.push_back(parseIfStmt());
        } else {
            elseBlock = parseBlock();
        }
    }
    
    return new IfStm(condition, thenBlock, elseBlock);
}

WhileStm* Parser::parseWhileStmt() {
    if (!match(Token::WHILE)) {
        throw runtime_error("Se esperaba 'while'");
    }
    
    Exp* condition = parseExpr();
    condition = condition->optimize();  // Optimizar condición
    Block* body = parseBlock();
    
    return new WhileStm(condition, body);
}

ForRangeStm* Parser::parseForRangeStmt() {
    if (!match(Token::FOR)) {
        throw runtime_error("Se esperaba 'for'");
    }
    
    if (!match(Token::ID)) {
        throw runtime_error("Se esperaba identificador de variable de iteración");
    }
    string loopVar = previous->text;
    
    if (!match(Token::IN)) {
        throw runtime_error("Se esperaba 'in'");
    }
    
    Exp* rangeStart = parseExpr();
    
    if (!match(Token::DOUBLE_DOT)) {
        throw runtime_error("Se esperaba '..' para rango");
    }
    
    Exp* rangeEnd = parseExpr();
    Block* body = parseBlock();
    
    return new ForRangeStm(loopVar, rangeStart, rangeEnd, body);
}

PrintlnStm* Parser::parsePrintlnStmt() {
    if (!match(Token::PRINTLN)) {
        throw runtime_error("Se esperaba 'println!'");
    }
    
    if (!match(Token::LPAREN)) {
        throw runtime_error("Se esperaba '(' después de println!");
    }
    
    PrintlnStm* stmt = new PrintlnStm("");
    
    // FormatArgs: STRING_LITERAL ',' ExprList | Expr | ε
    if (check(Token::STRING)) {
        match(Token::STRING);
        stmt->formatString = previous->text;
        
        if (match(Token::COMA)) {
            // ExprList
            stmt->args.push_back(parseExpr());
            while (match(Token::COMA)) {
                stmt->args.push_back(parseExpr());
            }
        }
    } else if (!check(Token::RPAREN)) {
        // Expr solo
        stmt->args.push_back(parseExpr());
    }
    // else: vacío println!()
    
    if (!match(Token::RPAREN)) {
        throw runtime_error("Se esperaba ')' después de argumentos de println!");
    }
    
    return stmt;
}

// ============================================
// Parsing de expresiones (precedencia)
// ============================================

Exp* Parser::parseExpr() {
    // Expr ::= IfExpr | LogicOr
    if (check(Token::IF)) {
        return parseIfExpr();
    }
    return parseLogicOr();
}

Exp* Parser::parseIfExpr() {
    if (!match(Token::IF)) {
        throw runtime_error("Se esperaba 'if'");
    }
    
    Exp* condition = parseExpr();
    Block* thenBlock = parseBlock();
    Block* elseBlock = nullptr;
    
    if (match(Token::ELSE)) {
        if (check(Token::IF)) {
            // else if encadenado
            Block* elseIfBlock = new Block();
            elseIfBlock->stmts.push_back(new ExprStm(parseIfExpr()));
            elseBlock = elseIfBlock;
        } else {
            elseBlock = parseBlock();
        }
    }
    
    return new IfExp(condition, thenBlock, elseBlock);
}

Exp* Parser::parseLogicOr() {
    Exp* left = parseLogicAnd();
    
    while (match(Token::OR)) {
        Exp* right = parseLogicAnd();
        left = new BinaryExp(left, right, OR_OP);
    }
    
    return left;
}

Exp* Parser::parseLogicAnd() {
    Exp* left = parseEquality();
    
    while (match(Token::AND)) {
        Exp* right = parseEquality();
        left = new BinaryExp(left, right, AND_OP);
    }
    
    return left;
}

Exp* Parser::parseEquality() {
    Exp* left = parseRelational();
    
    while (check(Token::EQ) || check(Token::NE)) {
        BinaryOp op = match(Token::EQ) ? EQ_OP : NE_OP;
        if (op == NE_OP) match(Token::NE);
        
        Exp* right = parseRelational();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parseRelational() {
    Exp* left = parseAdditive();
    
    while (check(Token::LT) || check(Token::LE) || 
           check(Token::GT) || check(Token::GE)) {
        BinaryOp op;
        if (match(Token::LT)) {
            op = LT_OP;
        } else if (match(Token::LE)) {
            op = LE_OP;
        } else if (match(Token::GT)) {
            op = GT_OP;
        } else {
            match(Token::GE);
            op = GE_OP;
        }
        
        Exp* right = parseAdditive();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parseAdditive() {
    Exp* left = parseMultiplicative();
    
    while (check(Token::PLUS) || check(Token::MINUS)) {
        BinaryOp op = match(Token::PLUS) ? PLUS_OP : MINUS_OP;
        if (op == MINUS_OP) match(Token::MINUS);
        
        Exp* right = parseMultiplicative();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parseMultiplicative() {
    Exp* left = parsePower();
    
    while (check(Token::MUL) || check(Token::DIV) || check(Token::MOD)) {
        BinaryOp op;
        if (match(Token::MUL)) {
            op = MUL_OP;
        } else if (match(Token::DIV)) {
            op = DIV_OP;
        } else {
            match(Token::MOD);
            op = MOD_OP;
        }
        
        Exp* right = parsePower();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parsePower() {
    Exp* left = parsePrefix();
    
    if (match(Token::POW)) {
        // Asociatividad derecha: recursivo
        Exp* right = parsePower();
        left = new BinaryExp(left, right, POW_OP);
    }
    
    return left;
}

Exp* Parser::parsePrefix() {
    // PrefixOp Prefix | CastExpr
    if (check(Token::NOT) || check(Token::MINUS) || 
        check(Token::AMP) || check(Token::MUL)) {
        
        UnaryOp op;
        if (match(Token::NOT)) {
            op = NOT_OP;
        } else if (match(Token::MINUS)) {
            op = NEG_OP;
        } else if (match(Token::AMP)) {
            // Podría ser & o &mut
            if (match(Token::MUT)) {
                op = REF_MUT_OP;
            } else {
                op = REF_OP;
            }
        } else {
            match(Token::MUL);
            op = DEREF_OP;
        }
        
        Exp* operand = parsePrefix(); // Recursivo
        return new UnaryExp(op, operand);
    }
    
    return parseCast();
}

Exp* Parser::parseCast() {
    Exp* expr = parsePostfix();
    
    // ('as' Type)*
    while (match(Token::AS)) {
        Type* targetType = parseType();
        expr = new CastExp(expr, targetType);
    }
    
    return expr;
}

Exp* Parser::parsePostfix() {
    Exp* expr = parsePrimary();
    
    // (PostfixTail)* → function calls
    while (check(Token::LPAREN)) {
        match(Token::LPAREN);
        
        // Convertir expr en FunCallExp si es ID
        IdExp* idExpr = expr->asIdExp();
        if (!idExpr) {
            throw runtime_error("Solo identificadores pueden ser llamados como funciones");
        }
        
        FunCallExp* callExpr = new FunCallExp(idExpr->id);
        delete idExpr; // Liberar el IdExp original
        
        // ArgListOpt
        if (!check(Token::RPAREN)) {
            callExpr->args.push_back(parseExpr());
            while (match(Token::COMA)) {
                callExpr->args.push_back(parseExpr());
            }
        }
        
        if (!match(Token::RPAREN)) {
            throw runtime_error("Se esperaba ')' después de argumentos");
        }
        
        expr = callExpr;
    }
    
    return expr;
}

Exp* Parser::parsePrimary() {
    // Literal | ID | '(' Expr ')'
    
    if (match(Token::NUM)) {
        return new NumberExp(stoll(previous->text));
    }
    
    if (match(Token::FLOAT)) {
        return new FloatExp(stod(previous->text));
    }
    
    if (match(Token::TRUE)) {
        return new BoolExp(true);
    }
    
    if (match(Token::FALSE)) {
        return new BoolExp(false);
    }
    
    if (match(Token::STRING)) {
        return new StringExp(previous->text);
    }
    
    if (match(Token::ID)) {
        return new IdExp(previous->text);
    }
    
    if (match(Token::LPAREN)) {
        Exp* expr = parseExpr();
        if (!match(Token::RPAREN)) {
            throw runtime_error("Se esperaba ')' después de expresión");
        }
        return new ParenExp(expr);
    }
    
    throw runtime_error("Se esperaba literal, identificador o '('");
}
