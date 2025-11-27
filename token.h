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
        
        
        UNSAFE,         // unsafe
        
        // Macros
        PRINTLN,        
        
        // Literales
        NUM,            
        FLOAT,          
        STRING,         
        
        // Identificadores
        ID,             // Identificador: suma, x, resultado
        
        
        SQRT,           
        FUN,            
        ENDFUN,         
        PRINT,          
        DO,           
        THEN,           
        ENDIF,          
        ENDWHILE,       
        VAR,           
        
        // Control
        ERR,            
        END             
    };

    // Atributos
    Type type;
    string text;
    int line;       // Número de línea
    int column;     // Número de columna

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& text, int line, int column);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
    
   
    static string typeToString(Type type);
};

#endif // TOKEN_H