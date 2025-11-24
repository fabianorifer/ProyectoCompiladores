#include "parser.h"
#include "ast.h" // ensure AST definitions visible after previous guard fix
#include <iostream>
#include <stdexcept>

using namespace std;

Parser::Parser(Scanner* scanner) : scanner(scanner) {
    current = scanner->nextToken();
    previous = nullptr;
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
        if (previous) delete previous;
        previous = current;
        current = scanner->nextToken();
        if (debug) cerr << "[ADV] token=" << previous->text << " type=" << previous->type << endl;
        if (current->type == Token::ERR) {
            cout << "Lexical Error: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return current->type == Token::END;
}

Token* Parser::consume(Token::Type type, string message) {
    if (check(type)) {
        advance();
        return previous;
    }
    cout << "Syntax Error: " << message << " Found: " << current->text << endl;
    exit(1);
}

Program* Parser::parseProgram() {
    Program* p = new Program();
    while (!isAtEnd()) {
        if (debug) cerr << "[PARSER] parseDeclaration inicio token=" << current->text << endl;
        p->addItem(parseDeclaration());
    }
    return p;
}

Node* Parser::parseDeclaration() {
    if (check(Token::FN)) return parseFunDec();
    if (check(Token::LET)) return parseVarDec(true);
    cout << "Error: se esperaba declaración (fn / let)" << endl;
    exit(1);
}

FunDec* Parser::parseFunDec() {
    consume(Token::FN, "se esperaba 'fn'");
    string name = consume(Token::ID, "se esperaba nombre de función")->text;
    if (debug) cerr << "[PARSER] FunDec nombre=" << name << endl;
    consume(Token::LPAREN, "se esperaba '('");
    vector<pair<string, DataType>> params = parseParams();
    consume(Token::RPAREN, "se esperaba ')'");
    DataType ret = DataType::VOID;
    if (match(Token::ARROW)) ret = parseType();
    bool prev = inFunction;
    inFunction = true;
    Block* body = parseBlock();
    inFunction = prev;
    return new FunDec(name, params, ret, body);
}

// Declaración variable (global o local) con o sin inferencia
VarDec* Parser::parseVarDec(bool global) {
    consume(Token::LET, "se esperaba 'let'");
    bool isMut = match(Token::MUT);
    string name = consume(Token::ID, "se esperaba identificador")->text;
    if (debug) cerr << "[PARSER] VarDec nombre=" << name << " global=" << (global?"si":"no") << endl;

    DataType type = DataType::UNKNOWN;
    Exp* init = nullptr;
    if (match(Token::COLON)) {
        type = parseType();
        if (match(Token::ASSIGN)) {
            init = parseExpression();
        }
    } else if (match(Token::ASSIGN)) { // inferencia
        init = parseExpression();
    } else {
        // Si no hay tipo y tampoco inicializador ⇒ error (inferencia requiere '=' )
        cout << "Error: inferencia requiere inicializador" << endl; exit(1);
    }
    consume(Token::SEMICOL, "se esperaba ';'");
    return new VarDec(name, type, init, isMut, global);
}

DataType Parser::parseType() {
    // Consumir prefijos de puntero; por ahora los tratamos como I64 (direcciones)
    bool hasPtr = false;
    while (check(Token::PTR_MUT) || check(Token::PTR_CONST)) { advance(); hasPtr = true; }
    if (match(Token::I32)) return hasPtr ? DataType::I64 : DataType::I32; // puntero a i32 -> dirección
    if (match(Token::I64)) return DataType::I64;
    if (match(Token::F32)) return hasPtr ? DataType::I64 : DataType::F32;
    if (match(Token::F64)) return hasPtr ? DataType::I64 : DataType::F64;
    if (match(Token::BOOL)) return hasPtr ? DataType::I64 : DataType::BOOL;
    cout << "Error: tipo desconocido" << endl; exit(1);
}

vector<pair<string, DataType>> Parser::parseParams() {
    vector<pair<string, DataType>> params;
    if (!check(Token::RPAREN)) {
        do {
            string name = consume(Token::ID, "se esperaba nombre de parámetro")->text;
            consume(Token::COLON, "se esperaba ':'");
            DataType t = parseType();
            params.push_back({name, t});
        } while (match(Token::COMA));
    }
    return params;
}

Block* Parser::parseBlock() {
    consume(Token::LBRACE, "se esperaba '{'");
    Block* b = new Block();
    while (!check(Token::RBRACE) && !isAtEnd()) {
        if (debug) cerr << "[PARSER] parseStatement dentro de bloque token=" << current->text << endl;
        b->addStm(parseStatement());
    }
    consume(Token::RBRACE, "se esperaba '}'");
    return b;
}

Stm* Parser::parseStatement() {
    if (check(Token::IF)) return parseIfStm();
    if (check(Token::WHILE)) return parseWhileStm();
    if (check(Token::FOR)) return parseForStm();
    if (check(Token::RETURN)) return parseReturnStm();
    if (check(Token::PRINTLN)) return parsePrintlnStm();
    if (check(Token::LBRACE)) return parseBlock();
    if (check(Token::LET)) return parseVarDec(false);
    if (debug) cerr << "[PARSER] parseStatement expresión token=" << current->text << endl;

    Exp* expr = parseExpression();
    Token* opToken = nullptr;
    if (match(Token::ASSIGN) || match(Token::PLUS_ASSIGN) || match(Token::MINUS_ASSIGN) ||
        match(Token::MUL_ASSIGN) || match(Token::DIV_ASSIGN) || match(Token::MOD_ASSIGN)) {
        opToken = previous;
        Exp* rhs = parseExpression();
        if (opToken->type != Token::ASSIGN) {
            BinaryOp bop;
            switch (opToken->type) {
                case Token::PLUS_ASSIGN: bop = BinaryOp::PLUS_OP; break;
                case Token::MINUS_ASSIGN: bop = BinaryOp::MINUS_OP; break;
                case Token::MUL_ASSIGN: bop = BinaryOp::MUL_OP; break;
                case Token::DIV_ASSIGN: bop = BinaryOp::DIV_OP; break;
                case Token::MOD_ASSIGN: bop = BinaryOp::MOD_OP; break;
                default: bop = BinaryOp::PLUS_OP; break;
            }
            rhs = new BinaryExp(expr, rhs, bop);
        }
        consume(Token::SEMICOL, "se esperaba ';'");
        return new AssignStm(expr, rhs);
    }
    if (inFunction && check(Token::RBRACE)) {
        // retorno implícito
        return new ReturnStm(expr);
    }
    consume(Token::SEMICOL, "se esperaba ';'");
    return new ExprStm(expr);
}

IfStm* Parser::parseIfStm() {
    consume(Token::IF, "Expected if");
    Exp* condition = parseExpression();
    Block* thenBlock = parseBlock();
    Stm* elseBlock = nullptr;
    if (match(Token::ELSE)) {
        if (check(Token::IF)) elseBlock = parseIfStm();
        else elseBlock = parseBlock();
    }
    return new IfStm(condition, thenBlock, elseBlock);
}

WhileStm* Parser::parseWhileStm() {
    consume(Token::WHILE, "Expected while");
    Exp* condition = parseExpression();
    Block* block = parseBlock();
    return new WhileStm(condition, block);
}

ForStm* Parser::parseForStm() {
    consume(Token::FOR, "Expected for");
    string var = consume(Token::ID, "Expected loop variable")->text;
    if (debug) cerr << "[PARSER] For loop var=" << var << endl;
    consume(Token::IN, "Expected in");
    Exp* start = parseExpression();
    consume(Token::DOTDOT, "Expected ..");
    Exp* end = parseExpression();
    Block* block = parseBlock();
    return new ForStm(var, start, end, block);
}

ReturnStm* Parser::parseReturnStm() {
    consume(Token::RETURN, "Expected return");
    Exp* expr = nullptr;
    if (!check(Token::SEMICOL)) {
        expr = parseExpression();
    }
    consume(Token::SEMICOL, "Expected ;");
    return new ReturnStm(expr);
}

PrintStm* Parser::parsePrintlnStm() {
    consume(Token::PRINTLN, "se esperaba println!");
    consume(Token::LPAREN, "se esperaba '('");
    vector<Exp*> args;
    string format = ""; // puede quedar vacío
    if (!check(Token::RPAREN)) {
        if (check(Token::STRING)) { // formato + opcionales expresiones
            format = consume(Token::STRING, "se esperaba string")->text;
            if (debug) cerr << "[PARSER] Print formato=" << format << endl;
            // El literal incluye comillas; removerlas para facilitar generación de ensamblador
            if (format.size() >= 2 && format.front() == '"' && format.back() == '"') {
                format = format.substr(1, format.size() - 2);
            }
            if (match(Token::COMA)) {
                args = parseArgs();
            }
        } else { // solo una expresión
            args.push_back(parseExpression());
        }
    }
    consume(Token::RPAREN, "se esperaba ')'");
    consume(Token::SEMICOL, "se esperaba ';'");
    return new PrintStm(format, args);
}

Exp* Parser::parseExpression() {
    return parseLogicOr();
}

Exp* Parser::parseLogicOr() {
    Exp* left = parseLogicAnd();
    while (match(Token::OR)) {
        Exp* right = parseLogicAnd();
        left = new BinaryExp(left, right, BinaryOp::OR_OP);
    }
    return left;
}

Exp* Parser::parseLogicAnd() {
    Exp* left = parseEquality();
    while (match(Token::AND)) {
        Exp* right = parseEquality();
        left = new BinaryExp(left, right, BinaryOp::AND_OP);
    }
    return left;
}

Exp* Parser::parseEquality() {
    Exp* left = parseComparison();
    while (check(Token::EQ) || check(Token::NEQ)) {
        BinaryOp op = match(Token::EQ) ? BinaryOp::EQ_OP : BinaryOp::NE_OP;
        Exp* right = parseComparison();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseComparison() {
    Exp* left = parseTerm();
    while (check(Token::LT) || check(Token::LE) || check(Token::GT) || check(Token::GE)) {
        BinaryOp op;
        if (match(Token::LT)) op = BinaryOp::LT_OP;
        else if (match(Token::LE)) op = BinaryOp::LE_OP;
        else if (match(Token::GT)) op = BinaryOp::GT_OP;
        else op = BinaryOp::GE_OP;
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (check(Token::PLUS) || check(Token::MINUS)) {
        BinaryOp op = match(Token::PLUS) ? BinaryOp::PLUS_OP : BinaryOp::MINUS_OP;
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFactor() {
    Exp* left = parsePower();
    while (check(Token::MUL) || check(Token::DIV) || check(Token::MOD)) {
        BinaryOp op;
        if (match(Token::MUL)) {
            op = BinaryOp::MUL_OP;
        } else if (match(Token::DIV)) {
            op = BinaryOp::DIV_OP;
        } else if (match(Token::MOD)) {
            op = BinaryOp::MOD_OP; // ahora consumimos correctamente '%'
        } else {
            // No debería ocurrir, evitar loop infinito
            cout << "Error interno: operador factor inesperado" << endl; exit(1);
        }
        Exp* right = parsePower();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parsePower() {
    Exp* left = parseUnary();
    while (match(Token::POW)) {
        Exp* right = parseUnary();
        left = new BinaryExp(left, right, BinaryOp::POW_OP);
    }
    return left;
}

Exp* Parser::parseUnary() {
    if (match(Token::MINUS)) return new UnaryExp(UnaryOp::MINUS_OP, parseUnary());
    if (match(Token::NOT)) return new UnaryExp(UnaryOp::NOT_OP, parseUnary());
    if (match(Token::MUL)) return new UnaryExp(UnaryOp::DEREF_OP, parseUnary());
    if (match(Token::AMP)) return new UnaryExp(UnaryOp::ADDR_OP, parseUnary());
    if (match(Token::AMP_MUT)) return new UnaryExp(UnaryOp::ADDR_MUT_OP, parseUnary());
    return parseCall();
}

Exp* Parser::parseCall() {
    Exp* expr = parsePrimary();
    while (true) {
        if (match(Token::LPAREN)) {
            vector<Exp*> args = parseArgs();
            consume(Token::RPAREN, "Expected )");
            if (IdExp* id = dynamic_cast<IdExp*>(expr)) {
                expr = new CallExp(id->id, args);
            } else {
                cout << "Error: Expected function name" << endl;
                exit(1);
            }
        } else if (match(Token::AS)) {
            DataType type = parseType();
            expr = new CastExp(expr, type);
        } else {
            break;
        }
    }
    return expr;
}

Exp* Parser::parsePrimary() {
    if (match(Token::FALSE)) return new BoolExp(false);
    if (match(Token::TRUE)) return new BoolExp(true);
    if (match(Token::NUM)) return new NumberExp(previous->text, DataType::I32);
    if (match(Token::FLOAT)) return new NumberExp(previous->text, DataType::F64);
    if (match(Token::STRING)) return new StringExp(previous->text);
    if (match(Token::ID)) return new IdExp(previous->text);
    if (match(Token::LPAREN)) {
        Exp* inner = parseExpression();
        consume(Token::RPAREN, "se esperaba ')'");
        return inner;
    }
    cout << "Error: token inesperado " << current->text << endl; exit(1);
}

vector<Exp*> Parser::parseArgs() {
    vector<Exp*> args;
    if (!check(Token::RPAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match(Token::COMA));
    }
    return args;
}
