#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        // Operadores aritméticos
        PLUS,       // +
        MINUS,      // -
        MUL,        // *
        DIV,        // /
        MOD,        // %
        POW,        // **
        
        // Operadores de comparación
        EQ,         // ==
        NEQ,        // !=
        LT,         // <
        LE,         // <=
        GT,         // >
        GE,         // >=
        
        // Operadores lógicos
        AND,        // &&
        OR,         // ||
        NOT,        // !
        
        // Operadores de asignación
        ASSIGN,     // =
        PLUS_ASSIGN,  // +=
        MINUS_ASSIGN, // -=
        MUL_ASSIGN,   // *=
        DIV_ASSIGN,   // /=
        MOD_ASSIGN,   // %=
        
        // Delimitadores
        LPAREN,     // (
        RPAREN,     // )
        LBRACE,     // {
        RBRACE,     // }
        SEMICOL,    // ;
        COMA,       // ,
        COLON,      // :
        ARROW,      // ->
        DOTDOT,     // ..
        
        // Keywords
        LET,        // let
        MUT,        // mut
        FN,         // fn
        RETURN,     // return
        IF,         // if
        ELSE,       // else
        WHILE,      // while
        FOR,        // for
        IN,         // in
        AS,         // as
        PRINTLN,    // println!
        
        // Tipos
        I32,        // i32
        I64,        // i64
        F32,        // f32
        F64,        // f64
        BOOL,       // bool
        
        // Punteros
        PTR_MUT,    // *mut
        PTR_CONST,  // *const
        AMP,        // &
        AMP_MUT,    // &mut
        DEREF,      // * (como operador unario de desreferencia)
        
        // Literales y otros
        NUM,        // Número entero
        FLOAT,      // Número flotante
        STRING,     // String literal
        TRUE,       // true
        FALSE,      // false
        ID,         // Identificador
        
        // Control
        ERR,        // Error
        END         // Fin de entrada
    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H