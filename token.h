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
        PLUS,           // +
        MINUS,          // -
        MUL,            // * (también usado para punteros)
        DIV,            // /
        MOD,            // %
        POW,            // ** (potencia)
        
        // Operadores de asignación
        ASSIGN,         // =
        PLUS_ASSIGN,    // +=
        MINUS_ASSIGN,   // -=
        MUL_ASSIGN,     // *=
        DIV_ASSIGN,     // /=
        MOD_ASSIGN,     // %=
        
        // Operadores relacionales
        LT,             // 
        GT,             // >
        LE,             // <=
        GE,             // >=
        EQ,             // ==
        NE,             // !=
        
        // Operadores lógicos
        AND,            // &&
        OR,             // ||
        NOT,            // !
        
        // Delimitadores
        LPAREN,         // (
        RPAREN,         // )
        LBRACE,         // {
        RBRACE,         // }
        SEMICOL,        // ;
        COMA,           // ,
        COLON,          // :
        ARROW,          // ->
        DOUBLE_DOT,     // ..
        
        // Operadores de referencia/punteros
        AMP,            // & (referencia)
        STAR_MUT,       // *mut
        STAR_CONST,     // *const
        
        // Palabras clave - control de flujo
        IF,             // if
        ELSE,           // else
        WHILE,          // while
        FOR,            // for
        IN,             // in
        RETURN,         // return
        
        // Palabras clave - declaraciones
        LET,            // let
        MUT,            // mut
        FN,             // fn
        
        // Palabras clave - tipos
        I32,            // i32
        I64,            // i64
        F32,            // f32
        F64,            // f64
        BOOL,           // bool
        
        // Palabras clave - literales booleanos
        TRUE,           // true
        FALSE,          // false
        
        // Palabras clave - conversión
        AS,             // as
        
        // Palabras clave - memoria (opcional)
        UNSAFE,         // unsafe
        
        // Macros
        PRINTLN,        // println!
        
        // Literales
        NUM,            // Número entero: 123, 456
        FLOAT,          // Número flotante: 3.14, 1.5e10
        STRING,         // String literal: "hello"
        
        // Identificadores
        ID,             // Identificador: suma, x, resultado
        
        // Tokens obsoletos (mantener por compatibilidad con código antiguo)
        SQRT,           // sqrt (deprecated)
        FUN,            // fun (deprecated - ahora es FN)
        ENDFUN,         // endfun (deprecated)
        PRINT,          // print (deprecated - ahora es PRINTLN)
        DO,             // do (deprecated)
        THEN,           // then (deprecated)
        ENDIF,          // endif (deprecated)
        ENDWHILE,       // endwhile (deprecated)
        VAR,            // var (deprecated - ahora es LET)
        
        // Control
        ERR,            // Token de error
        END             // Fin de entrada
    };

    // Atributos
    Type type;
    string text;
    int line;       // Número de línea (opcional, útil para errores)
    int column;     // Número de columna (opcional)

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& text, int line, int column);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
    
    // Método auxiliar para obtener nombre del token (útil para debugging)
    static string typeToString(Type type);
};

#endif // TOKEN_H