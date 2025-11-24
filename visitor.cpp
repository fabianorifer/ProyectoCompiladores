#include "visitor.h"
#include "ast.h"
#include <iostream>
#include <algorithm> // std::remove
// #include <vector> // removed need; no dynamic containers now

// Refresh after AST include guard change

using namespace std;

int GenCodeVisitor::visit(BinaryExp* exp) {
    // Evaluar left y guardar en r10, luego copiar a scratch [rbp+scratchOffset]
    // (slot dedicado) para preservarlo en presencia de llamadas.
    exp->left->accept(this);
    out << "  mov r10, rax" << endl;
    out << "  mov [rbp" << scratchOffset << "], r10" << endl;
    // Evaluar right
    exp->right->accept(this);
    // Recuperar left
    out << "  mov r10, [rbp" << scratchOffset << "]" << endl;
    bool leftFloat = false;
    bool rightFloat = false;
    // Usar visitor para detectar tipo sin dynamic_cast
    TypeCheckVisitor tcLeft, tcRight;
    exp->left->accept(&tcLeft);
    exp->right->accept(&tcRight);
    if (tcLeft.isNumberExp) leftFloat = tcLeft.isFloatNumber;
    if (tcLeft.isIdExp) {
        auto itL = vars.find(tcLeft.idName);
        if (itL != vars.end() && (itL->second.type == DataType::F64 || itL->second.type == DataType::F32)) leftFloat = true;
    }
    if (tcLeft.isCastExp) {
        if (tcLeft.castTarget == DataType::F64 || tcLeft.castTarget == DataType::F32) leftFloat = true;
    }
    if (tcRight.isNumberExp) rightFloat = tcRight.isFloatNumber;
    if (tcRight.isIdExp) {
        auto itR = vars.find(tcRight.idName);
        if (itR != vars.end() && (itR->second.type == DataType::F64 || itR->second.type == DataType::F32)) rightFloat = true;
    }
    if (tcRight.isCastExp) {
        if (tcRight.castTarget == DataType::F64 || tcRight.castTarget == DataType::F32) rightFloat = true;
    }
    switch (exp->op) {
        case BinaryOp::PLUS_OP:
            out << "  add rax, r10" << endl; break;
        case BinaryOp::MINUS_OP: {
            // Normalizar doble negación si el parser produjo right = Unary(-, Number)
            if (tcRight.isUnaryExp && tcRight.unaryOp == UnaryOp::MINUS_OP) {
                // right actualmente en rax es -(operand); revertir a +(operand)
                out << "  neg rax" << endl;
            }
            out << "  mov rcx, r10" << endl;
            out << "  sub rcx, rax" << endl;
            out << "  mov rax, rcx" << endl; break; }
        case BinaryOp::MUL_OP:
            out << "  imul rax, r10" << endl; break;
        case BinaryOp::DIV_OP: {
            if (leftFloat || rightFloat) {
                // División en doble precisión: xmm0 = left, xmm1 = right (mantener cada operando distinto)
                // left original preservado en r10, right actual en rax
                if (leftFloat) {
                    out << "  movq xmm0, r10" << endl; // r10 contiene bits double
                } else {
                    out << "  mov rax, r10" << endl;
                    out << "  cvtsi2sd xmm0, rax" << endl;
                }
                if (rightFloat) {
                    out << "  movq xmm1, rax" << endl; // rax contiene bits double del right
                } else {
                    out << "  cvtsi2sd xmm1, rax" << endl;
                }
                out << "  divsd xmm0, xmm1" << endl;
                out << "  movq rax, xmm0" << endl; // resultado vuelve a rax para código uniforme
            } else {
                out << "  mov r11, rax" << endl;
                out << "  mov rax, r10" << endl;
                out << "  cqo" << endl;
                out << "  idiv r11" << endl;
            }
            break; }
        case BinaryOp::MOD_OP: {
            out << "  mov r11, rax" << endl;
            out << "  mov rax, r10" << endl;
            out << "  cqo" << endl;
            out << "  idiv r11" << endl;
            out << "  mov rax, rdx" << endl; break; }
        case BinaryOp::POW_OP: {
            int id = labelCounter++;
            out << "  mov rcx, rax" << endl; // exponente
            out << "  mov rax, r10" << endl; // base
            out << "  mov rdx, 1" << endl;
            out << "  cmp rcx, 0" << endl;
            out << "  je L_pow_end" << id << endl;
            out << "L_pow_loop" << id << ":" << endl;
            out << "  imul rdx, rax" << endl;
            out << "  dec rcx" << endl;
            out << "  jne L_pow_loop" << id << endl;
            out << "L_pow_end" << id << ":" << endl;
            out << "  mov rax, rdx" << endl; break; }
        case BinaryOp::EQ_OP: case BinaryOp::NE_OP: case BinaryOp::LT_OP: case BinaryOp::LE_OP: case BinaryOp::GT_OP: case BinaryOp::GE_OP: {
            // cmp destino, fuente evalúa destino - fuente. Para left < right necesitamos left - right < 0.
            // Entonces: cmp left(r10), right(rax) y luego setl detecta si r10 < rax correctamente.
            out << "  cmp r10, rax" << endl;
            string setInstr;
            switch (exp->op) {
                case BinaryOp::EQ_OP: setInstr = "sete"; break;
                case BinaryOp::NE_OP: setInstr = "setne"; break;
                case BinaryOp::LT_OP: setInstr = "setl"; break;
                case BinaryOp::LE_OP: setInstr = "setle"; break;
                case BinaryOp::GT_OP: setInstr = "setg"; break;
                case BinaryOp::GE_OP: setInstr = "setge"; break;
                default: break;
            }
            out << "  " << setInstr << " al" << endl;
            out << "  movzx rax, al" << endl; break; }
        case BinaryOp::AND_OP: {
            int id = labelCounter++;
            out << "  cmp r10, 0" << endl;
            out << "  je L_and_false" << id << endl;
            out << "  cmp rax, 0" << endl;
            out << "  je L_and_false" << id << endl;
            out << "  mov rax, 1" << endl;
            out << "  jmp L_and_end" << id << endl;
            out << "L_and_false" << id << ":" << endl;
            out << "  xor rax, rax" << endl;
            out << "L_and_end" << id << ":" << endl; break; }
        case BinaryOp::OR_OP: {
            int id = labelCounter++;
            out << "  cmp r10, 0" << endl;
            out << "  jne L_or_true" << id << endl;
            out << "  cmp rax, 0" << endl;
            out << "  jne L_or_true" << id << endl;
            out << "  xor rax, rax" << endl;
            out << "  jmp L_or_end" << id << endl;
            out << "L_or_true" << id << ":" << endl;
            out << "  mov rax, 1" << endl;
            out << "L_or_end" << id << ":" << endl; break; }
        default: break;
    }
    return 0;
}

int GenCodeVisitor::visit(UnaryExp* exp) {
    exp->operand->accept(this);
    switch (exp->op) {
        case UnaryOp::MINUS_OP: out << "  neg rax" << endl; break;
        case UnaryOp::NOT_OP: 
            out << "  cmp rax, 0" << endl;
            out << "  sete al" << endl;
            out << "  movzx rax, al" << endl;
            break;
        case UnaryOp::DEREF_OP:
            out << "  mov rax, [rax]" << endl; // load value pointed by rax
            break;
        case UnaryOp::ADDR_OP:
        case UnaryOp::ADDR_MUT_OP: {
            // operand result in rax currently contains VALUE; need address if IdExp
            TypeCheckVisitor tcOperand;
            exp->operand->accept(&tcOperand);
            if (tcOperand.isIdExp) {
                auto it = vars.find(tcOperand.idName);
                if (it != vars.end()) {
                    if (it->second.global) out << "  lea rax, [rel " << tcOperand.idName << "]" << endl;
                    else out << "  lea rax, [rbp" << it->second.offset << "]" << endl;
                }
            } else {
                out << "  ; & aplicado a expresión no-id (sin dirección real)" << endl;
                // Mantener rax como estaba
            }
            break;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    if (exp->value.find('.') != string::npos) {
        int id = labelCounter++;
        out << "section .data" << endl;
        out << "numFloat" << id << ": dq " << exp->value << endl;
        out << "section .text" << endl;
        out << "  mov rax, [rel numFloat" << id << "]" << endl;
    } else {
        out << "  mov rax, " << exp->value << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << "  mov rax, " << (exp->value ? 1 : 0) << endl;
    return 0;
}

int GenCodeVisitor::visit(StringExp* exp) {
    // Handle strings (needs data section support)
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    auto it = vars.find(exp->id);
    if (it != vars.end()) {
        if (it->second.global) {
            out << "  mov rax, [rel " << exp->id << "]" << endl;
        } else {
            out << "  mov rax, [rbp" << it->second.offset << "]" << endl; // offset ya negativo formateado abajo
        }
    } else {
        out << "  xor rax, rax ; desconocido " << exp->id << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(CallExp* exp) {
    static const char* regOrder[] = {"rdi","rsi","rdx","rcx","r8","r9"};
    int idx = 0;
    for (auto arg : exp->args) {
        arg->accept(this); // resultado en rax
        if (idx < 6) {
            out << "  mov " << regOrder[idx] << ", rax" << endl;
        } else {
            // más de 6 argumentos: colocar en stack (simplificado)
            out << "  push rax" << endl;
        }
        idx++;
    }
    int stackExtras = idx > 6 ? (idx - 6) : 0;
    // Alineación SysV: antes del CALL queremos rsp%16 == 8.
    // Offset total desde el rsp original al entrar (antes de push rbp) es:
    // 8 (push rbp) + currentStackSize + stackExtras*8.
    // Si (8 + currentStackSize + stackExtras*8) %16 == 0 entonces rsp%16 == 0 y necesitamos restar 8.
    bool needPad = (((8 + currentStackSize + stackExtras*8) % 16) == 0);
    if (needPad) out << "  sub rsp, 8" << endl;
    out << "  call " << exp->funcName << endl;
    if (stackExtras > 0) out << "  add rsp, " << (stackExtras * 8) << endl;
    if (needPad) out << "  add rsp, 8" << endl;
    return 0;
}

int GenCodeVisitor::visit(CastExp* exp) {
    exp->expr->accept(this);
    // Conversiones básicas enteros <-> double
    if (exp->targetType == DataType::F64 || exp->targetType == DataType::F32) {
        // rax contiene entero -> convertir a double en xmm0 y regresar bits en rax
        out << "  cvtsi2sd xmm0, rax" << endl;
        out << "  movq rax, xmm0" << endl;
    } else {
        // Si origen era float (heurística: rax proviene de NumberExp con '.') convertir a entero truncado
        // Para simplicidad omitimos detección y asumimos ya es entero
    }
    return 0;
}

int GenCodeVisitor::visit(Block* stm) {
    int startStack = currentStackSize;
    bool isTopLevelFuncBody = (blockNesting == 0);
    blockNesting++;
    for (auto s : stm->stmts) {
        s->accept(this);
    }
    blockNesting--;
    if (!isTopLevelFuncBody) {
        int freed = currentStackSize - startStack;
        if (freed > 0) {
            out << "  add rsp, " << freed << endl;
            currentStackSize = startStack;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    if (stm->isGlobal) {
        // Defer definición global: etiqueta en .data si init constante
        if (stm->init) {
            TypeCheckVisitor tcInit;
            stm->init->accept(&tcInit);
            if (tcInit.isNumberExp) {
                out << "section .data" << endl;
                out << stm->name << ": dq " << tcInit.numberValue << endl;
                out << "section .text" << endl;
            } else if (tcInit.isBoolExp) {
                // BoolExp - usar valor extraído
                out << "section .data" << endl;
                out << stm->name << ": dq " << (tcInit.boolValue ? 1 : 0) << endl;
                out << "section .text" << endl;
            } else {
                out << "section .data" << endl;
                out << stm->name << ": dq 0" << endl;
                out << "section .text" << endl;
            }
        } else {
            out << "section .data" << endl;
            out << stm->name << ": dq 0" << endl;
            out << "section .text" << endl;
        }
        vars[stm->name] = {true, 0, stm->type};
    } else {
        // Reservar espacio en stack (8 bytes)
        currentStackSize += 8;
        int offset = -currentStackSize;
        // reservar inmediatamente
        out << "  sub rsp, 8" << endl;
        vars[stm->name] = {false, offset, stm->type};
        if (stm->init) {
            stm->init->accept(this);
            out << "  mov [rbp" << offset << "], rax" << endl;
        } else {
            out << "  mov qword [rbp" << offset << "], 0" << endl;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(AssignStm* stm) {
    // Primero detectar el tipo de place para manejar deref especialmente
    TypeCheckVisitor tcPlace;
    stm->place->accept(&tcPlace);
    
    if (tcPlace.isUnaryExp && tcPlace.unaryOp == UnaryOp::DEREF_OP) {
        // UnaryExp con DEREF_OP - necesita tratamiento especial
        if (getenv("COMP_DEBUG")) out << "  ; assign via * (deref)" << endl;
        // Evaluar operand para obtener dirección
        if (tcPlace.unaryOperand) {
            tcPlace.unaryOperand->accept(this); // dirección en rax
            out << "  push rax" << endl; // guardar dirección en stack
            // Evaluar expresión a asignar
            stm->expr->accept(this); // valor final en rax
            out << "  pop rcx" << endl; // recuperar dirección en rcx (caller-saved)
            out << "  mov [rcx], rax" << endl;
        } else {
            out << "  ; ERROR: UnaryExp DEREF sin operand" << endl;
        }
        return 0;
    }
    
    // Evaluar expresión primero
    stm->expr->accept(this); // resultado en rax
    
    if (tcPlace.isIdExp) {
        auto it = vars.find(tcPlace.idName);
        if (it != vars.end()) {
            if (getenv("COMP_DEBUG")) out << "  ; assign a id " << tcPlace.idName << (it->second.global?" (global)":" (local)") << endl;
            if (it->second.global) {
                out << "  mov [rel " << tcPlace.idName << "], rax" << endl;
            } else {
                out << "  mov [rbp" << it->second.offset << "], rax" << endl;
            }
        } else {
            out << "  ; asignación a desconocido " << tcPlace.idName << endl;
        }
    } else {
        out << "  ; asignación no soportada" << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    int id = labelCounter++;
    stm->condition->accept(this);
    out << "  cmp rax, 0" << endl;
    out << "  je L_if_else" << id << endl;
    stm->thenBlock->accept(this);
    out << "  jmp L_if_end" << id << endl;
    out << "L_if_else" << id << ":" << endl;
    if (stm->elseBlock) stm->elseBlock->accept(this);
    out << "L_if_end" << id << ":" << endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int id = labelCounter++;
    out << "L_while_start" << id << ":" << endl;
    stm->condition->accept(this);
    out << "  cmp rax, 0" << endl;
    out << "  je L_while_end" << id << endl;
    stm->block->accept(this);
    out << "  jmp L_while_start" << id << endl;
    out << "L_while_end" << id << ":" << endl;
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->expr) stm->expr->accept(this);
    // Saltar al epílogo unificado
    out << "  jmp " << currentFunctionEndLabel << endl;
    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    static int strId = 0;
    // Preparar formato y argumentos múltiples
    string fmt = stm->format;
    // Contar placeholders {}
    int placeholderCount = 0;
    if (fmt.empty()) {
        placeholderCount = (int)stm->args.size();
        if (placeholderCount == 0) fmt = "%lld"; // simple newline
        else {
            // construir formato concatenando %lld por cada arg
            fmt.clear();
            for (size_t i=0;i<stm->args.size();++i) {
                fmt += "%lld";
                if (i+1<stm->args.size()) fmt += " ";
            }
        }
    } else {
        size_t pos=0;
        while ((pos = fmt.find("{}", pos)) != string::npos) {
            fmt.replace(pos, 2, "%lld");
            pos += 4;
            placeholderCount++;
        }
    }
    // Eliminar saltos de línea internos
    fmt.erase(std::remove(fmt.begin(), fmt.end(), '\n'), fmt.end());
    int myId = strId++;
    out << "section .data" << endl;
    out << "fmt" << myId << ": db \"";
    for (char c : fmt) {
        if (c == '"') out << "\\\""; else out << c;
    }
    out << "\",10,0" << endl; // newline y terminador
    out << "section .text" << endl;
    // Evaluar argumentos primero; cargar formato en rdi justo antes del call
    // Mapeo de registros según SysV para argumentos adicionales
    static const char* intArgRegs[] = {"rsi","rdx","rcx","r8","r9"};
    int usedInt = 0;
    // Evaluar hasta placeholderCount argumentos; si faltan args, usar 0
    // Para mantener orden correcto en stack (derecha a izquierda), evaluar primero args en registro
    int stackArgs = placeholderCount > 5 ? placeholderCount - 5 : 0;
    // Primero procesar los que irán en registros (0..4)
    int regPlaceholders = placeholderCount < 5 ? placeholderCount : 5;
    for (int i=0; i<regPlaceholders; ++i) {
        if (i < (int)stm->args.size()) stm->args[i]->accept(this); else out << "  xor rax, rax" << endl;
        out << "  mov " << intArgRegs[usedInt] << ", rax" << endl;
        usedInt++;
    }
    // Los restantes (si hay) se empujan en orden inverso
    if (stackArgs > 0) {
        for (int i=placeholderCount-1; i>=5; --i) {
            if (i < (int)stm->args.size()) stm->args[i]->accept(this); else out << "  xor rax, rax" << endl;
            out << "  push rax" << endl;
        }
    }
    // Alineación antes del call printf: usar misma regla que en CallExp
    bool needPad = (((8 + currentStackSize + stackArgs*8) % 16) == 0);
    if (needPad) out << "  sub rsp, 8" << endl;
    // Cargar formato ahora que no habrá más llamadas antes del printf
    out << "  lea rdi, [rel fmt" << myId << "]" << endl;
    // AL = número de registros SSE usados (0, no soportamos float varargs por ahora)
    out << "  xor eax, eax" << endl;
    out << "  call printf" << endl;
    // Limpieza de stack si hubo argumentos >5
    if (stackArgs > 0) out << "  add rsp, " << (stackArgs * 8) << endl;
    if (needPad) out << "  add rsp, 8" << endl;
    return 0;
}

int GenCodeVisitor::visit(ExprStm* stm) {
    stm->expr->accept(this);
    return 0;
}

int GenCodeVisitor::visit(FunDec* stm) {
    // Preservar variables globales antes de limpiar locales
    unordered_map<string, VarInfo> savedGlobals;
    for (auto &kv : vars) {
        if (kv.second.global) savedGlobals[kv.first] = kv.second;
    }
    vars.clear();
    vars = savedGlobals;
    out << stm->name << ":" << endl;
    out << "  push rbp" << endl;
    out << "  mov rbp, rsp" << endl;
    // Reservar alineación inicial si se necesitarán llamadas. Ajustaremos mediante padding dinámico.
    // Comenzamos con currentStackSize = 0 (sin sub adicional) y solo reservamos scratch.
    currentStackSize = 0;
    // Reservar scratch persistente (8 bytes). Esto puede desalinear; manejaremos padding antes de cada call.
    currentStackSize += 8; scratchOffset = -currentStackSize;
    out << "  sub rsp, 8" << endl; // espacio scratch
    if (getenv("COMP_DEBUG")) out << "  ; entrando función " << stm->name << endl;
    // Mapear parámetros (solo enteros por ahora)
    static const char* regOrder[] = {"rdi","rsi","rdx","rcx","r8","r9"};
    int paramCount = (int)stm->params.size();
    for (int i=0;i<paramCount && i<6;i++) {
        currentStackSize += 8;
        out << "  sub rsp, 8" << endl;
        int offset = -currentStackSize;
        vars[stm->params[i].first] = {false, offset, stm->params[i].second};
        out << "  mov [rbp" << offset << "], " << regOrder[i] << endl;
        if (getenv("COMP_DEBUG")) out << "  ; param " << stm->params[i].first << " -> [rbp" << offset << "]" << endl;
    }
    // Definir etiqueta de fin
    int endId = labelCounter++;
    currentFunctionEndLabel = "L_func_end" + std::to_string(endId);
    stm->body->accept(this);
    out << currentFunctionEndLabel << ":" << endl;
    if (stm->name == "main") {
        // Garantizar código de salida 0 si main no retornó explícitamente
        out << "  xor eax, eax" << endl;
    }
    if (currentStackSize > 0) {
        out << "  add rsp, " << currentStackSize << endl; // liberar stack
    }
    out << "  pop rbp" << endl;
    out << "  ret" << endl;
    return 0;
}

int GenCodeVisitor::visit(Program* stm) {
    out << "global main" << endl;
    out << "extern printf" << endl;
    out << "section .text" << endl;
    for (auto item : stm->items) {
        item->accept(this);
    }
    return 0;
}

int GenCodeVisitor::visit(ForStm* stm) {
    int id = labelCounter++;
    currentStackSize += 8; out << "  sub rsp, 8" << endl; int offset = -currentStackSize;
    vars[stm->var] = {false, offset, DataType::I64};
    stm->start->accept(this); // rax = start
    out << "  mov [rbp" << offset << "], rax" << endl;
    out << "L_for_begin" << id << ":" << endl;
    out << "  mov rax, [rbp" << offset << "]" << endl;
    out << "  push rax" << endl;
    stm->end->accept(this); // rax = end
    out << "  pop r11" << endl; // r11 = i (caller-saved, no requiere preservar)
    out << "  cmp r11, rax" << endl;
    out << "  jge L_for_end" << id << endl;
    // cuerpo
    stm->block->accept(this);
    // i++
    out << "  mov rax, [rbp" << offset << "]" << endl;
    out << "  inc rax" << endl;
    out << "  mov [rbp" << offset << "], rax" << endl;
    out << "  jmp L_for_begin" << id << endl;
    out << "L_for_end" << id << ":" << endl;
    return 0;
}

void GenCodeVisitor::generar(Program* p) {
    if (!p) return;
    p->accept(this);
}
