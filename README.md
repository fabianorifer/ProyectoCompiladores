# Subconjunto Rust-like extendido para Compilador x86-64

Este documento define la gramática y lineamientos semánticos del lenguaje objetivo (inspirado en Rust) que el compilador implementará. Incluye:
- Declaración de variables (mutables / inmutables) con inferencia.
- Operaciones aritméticas y de relación.
- Sentencias de control `if`, `while`, `for` con rangos.
- Definición y uso de funciones.
- Tipos y coerciones: `i32`, `i64`, `f32`, `f64`, `bool`, punteros crudos `*mut T`, `*const T`.
- Conversión (`as`) y promoción de tipos numéricos.
- Inferencia de tipos en `let`.

Excluidos deliberadamente (por simplicidad actual): referencias (`&` / `&mut` con reglas de préstamo) y literales de cadena.

---
## 1. Tokens (Resumen)
Palabras clave:
```
fn let mut return if else while for in true false as
```
Tipos básicos:
```
i32 i64 f32 f64 bool
```
Punteros (prefijos):
```
*mut *const
```
Operadores:
```
+ - * / % ** == != < <= > >= && || ! & * = ..
```
Delimitadores:
```
( ) { } , ; ->
```
Identificadores: `ID = [a-zA-Z_][a-zA-Z0-9_]*`  
Enteros: `[0-9]+`  
Flotantes: `[0-9]+ '.' [0-9]+`  
String literal: `"..."` con escapes `\n`, `\"`, `\\`  
Comentarios sugeridos: `// ...` ignorados.

`..` es un token de rango exclusivo (inicio .. fin).

---
## 2. Precedencia (menor → mayor)
1. `||`
2. `&&`
3. Comparación: `== != < <= > >=`
4. Aditivos: `+ -`
5. Multiplicativos: `* / %`
6. Potencia: `**` (derecha)
7. Prefijos unarios: `! - & *`
8. Cast: `as`
9. Primarios y postfix (llamadas) / if expresión

---
## 3. Gramática (EBNF)
`?` opcional, `*` cero o más, `+` uno o más. Solo las sentencias "simples" (declaraciones, asignaciones, return y expresiones) requieren punto y coma; las de control (`if`, `while`, `for`, bloques) no.

```
Program        ::= ItemList
ItemList       ::= (Item)*
Item           ::= FunDecl | GlobalVarDecl

GlobalVarDecl  ::= 'let' Mutability? ID TypeAnnotation InitializerOpt ';'
                | 'let' Mutability? ID '=' Expr ';'               // inferencia global requiere inicializador

FunDecl        ::= 'fn' ID '(' ParamListOpt ')' ReturnTypeOpt Block
ParamListOpt   ::= ParamList?
ParamList      ::= Param (',' Param)*
Param          ::= ID TypeAnnotation         // sin inferencia de parámetros
ReturnTypeOpt  ::= ('->' Type)?

Mutability     ::= 'mut'
TypeAnnotation ::= ':' Type
InitializerOpt ::= '=' Expr | ε

Type           ::= BaseType | PointerType
BaseType       ::= 'i32' | 'i64' | 'f32' | 'f64' | 'bool'
PointerType    ::= PointerPrefix Type                 // punteros recursivos (a punteros)
PointerPrefix  ::= '*mut' | '*const'

Block          ::= '{' StmtListOpt '}'
StmtListOpt    ::= StmtList?
StmtList       ::= (Stmt)*

Stmt           ::= SimpleStmt ';'
                | IfExpr
                | WhileStmt
                | ForRangeStmt
                | Block
                | PrintlnStmt ';'

PrintlnStmt    ::= 'println!' '(' STRING_LITERAL ',' Expr ')'
                | 'println!' '(' Expr ')'

SimpleStmt     ::= VarDecl
                | AssignStmt
                | ReturnStmt
                | ExprStmt

VarDecl        ::= 'let' Mutability? ID TypeAnnotation InitializerOpt
                | 'let' Mutability? ID '=' Expr         // inferencia local, obliga inicializador
AssignStmt     ::= PlaceExpr AssignOp Expr    
AssignOp       ::= '=' | '+=' | '-=' | '*=' | '/='  | '%='
PlaceExpr      ::= ID | DerefExpr
DerefExpr      ::= '*' Expr

PrintlnStmt    ::= 'println!' '(' FormatArgs ')' 
FormatArgs     ::= STRING_LITERAL ',' ExprList                    // ← println!("{} {}", x, y)
                | Expr                                            // ← println!(x)
                | ε                                               // ← println!()
ExprList       ::= Expr (',' Expr)*                                              

ForRangeStmt   ::= 'for' ID 'in' Expr '..' Expr Block   // rango: inicio <= i < fin
WhileStmt      ::= 'while' Expr Block
ReturnStmt     ::= 'return' ExprOpt
ExprOpt        ::= Expr?
ExprStmt       ::= Expr

Expr           ::= IfExpr | LogicOr
IfExpr         ::= 'if' Expr Block ElsePartOpt        // si se usa en contexto que requiere valor y falta else ⇒ error
ElsePartOpt    ::= ('else' (Block | IfExpr))?

LogicOr        ::= LogicAnd ( '||' LogicAnd )*
LogicAnd       ::= Equality ( '&&' Equality )*
Equality       ::= Rel ( ( '==' | '!=' ) Rel )*
Rel            ::= Add ( ( '<' | '<=' | '>' | '>=' ) Add )*
Add            ::= Mul ( ( '+' | '-' ) Mul )*
Mul            ::= Pow ( ( '*' | '/' | '%' ) Pow )*
Pow            ::= Prefix ( '**' Pow )?               // derecha
Prefix         ::= PrefixOp Prefix | CastExpr
PrefixOp       ::= '!' | '-' | '&' | '&mut' | '*' 

CastExpr       ::= Postfix ('as' Type)*               // permite cadenas: x as i64 as f64
Postfix        ::= Primary (PostfixTail)*
PostfixTail    ::= '(' ArgListOpt ')'                 // llamadas

Primary        ::= Literal | ID | '(' Expr ')'
ArgListOpt     ::= ArgList?
ArgList        ::= Expr (',' Expr)*

Literal        ::= INT_LITERAL | FLOAT_LITERAL | BoolLiteral | STRING_LITERAL
BoolLiteral    ::= 'true' | 'false'
STRING_LITERAL ::= '"' ( [^"\\] | \\ . )* '"'
INT_LITERAL    ::= [0-9]+
FLOAT_LITERAL  ::= [0-9]+ '.' [0-9]+ ([eE] [+-]? [0-9]+)?        // ← notación científica
ID             ::= [a-zA-Z_][a-zA-Z0-9_]*
```

Nota: Los punteros pueden anidarse: `*mut *const i32` es válido por la producción recursiva.

---
## 4. Modelo de Tipos
Sugerencia de representación interna:
```
enum class TypeKind {
  Int32, Int64,
  Float32, Float64,
  Bool,
  PtrConst, PtrMut // 'to' puede ser otro puntero (recursivo)
};
struct Type { TypeKind kind; Type* to; };
```

---
## 5. Reglas de Inferencia y Promoción
- Entero literal: inicial `i32`; se promociona a `i64`, `f32` o `f64` según contexto.
- Flotante literal: `f64` por defecto (similar a Rust); cast explícito a `f32` si se requiere menor precisión.
- Promociones implícitas permitidas:
  - i32 → i64
  - i32 → f32 / f64
  - i64 → f64
  - f32 → f64
- `f32` no se degrada a `i32/i64` sin `as`.
- Cast descendente obligatorio para reducir tamaño o precisión (`f64 as f32`, `i64 as i32`).
- `Expr as Type` crea nodo `Cast` validado por el analizador semántico.
- Dirección `&x` ⇒ `*mut T` si `x` es `mut`, `*const T` si no; (sin modelo de préstamos).
- Deref `*p` ⇒ tipo base de `p` (error si no pointer).
- Aritmética de punteros (opcional): `ptr + entero` ajusta por tamaño base; se validará en semántica.
- Comparaciones numéricas: ambos lados se elevan al supertipo común.
- Operador `%` solo enteros.
- Operador `**`: si algún operando es flotante ⇒ resultado flotante (promoción). Si ambos enteros ⇒ más ancho (i64 si cualquiera i64).
- `if` expresión: ramas unificadas por promociones; punteros deben coincidir exactamente.
- For-range: límites deben ser enteros; tipo de la variable de iteración = supertipo entero (i64 si uno es i64); flotantes producen error.
- Booleanos no se autoconvierten a enteros; usar cast explícito si se requiere (`flag as i32`).
- Inferencia: prohibida sin inicializador. Si falta tipo y falta `=` se reporta error.

---
## 6. Errores Semánticos Clave
- Uso de identificador no declarado.
- Asignación con tipos incompatibles sin cast.
- Dereferenciar valor no pointer.
- Tomar dirección de literal o temporal (`&5`).
- Rango `for` con límites no enteros.
- Uso de `%` con operandos flotantes.
- Cast inválido (e.g. pointer a flotante sin conversión permitida).
- Declaración inferida sin inicializador (`let x;`).

---
## 7. Notas para Codegen x86-64
- `i32`: almacenar en 4 bytes (alineado a 8 si simplifica stack). Extensión sign con `movslq` al usar en operaciones 64 bits.
- `i64`: registros generales (`%rax`, `%rbx`, etc.).
- `f32`: usar registros XMM y mantener conversión con `cvtsi2ss` / `cvtss2sd` cuando se mezcla con `f64`.
- `f64`: registros XMM (`%xmm0`...).
- Cast int→f64: `cvtsi2sd`; int→f32: `cvtsi2ss`.
- Cast f32→f64: `cvtss2sd`; f64→f32: `cvtsd2ss`.
- Cast f64→int: `cvttsd2si`; f32→int: `cvttss2si`.
- Punteros: tratados como enteros 64 bits.
- `for` rango: expandir a inicialización + while con comparación `<`.

---
## 8. Ejemplos
Inferencia y promociones:
```
let a = 5;              // a: i32
let b = a + 2i64;       // b: i64 (promoción de a)
let c = b as f64;       // c: f64
let d: f32 = c as f32;  // d: f32 (reducción explícita)
let e = a as i64 as f64; // cadena de casts
let mut p: *mut i32 = &a; // puntero mutable
let q: *const *mut i32 = &p; // puntero a puntero
```
For-range:
```
for i in 0 .. 10 {
    // i: i32
}
```
Punteros:
```
let mut x: i32 = 10;
let p = &x;         // p: *mut i32
*p = *p + 1;
```
If expresión / sentencia:
```
// Como expresión (necesita else para producir valor):
let sign = if x < 0 { -1 } else { 1 };

// Como sentencia (valor unit implícito, sin ; al final):
if sign < 0 { return; }
```
Función:
```
fn add_i64(a: i64, b: i64) -> i64 { a + b }
fn main() {
    let mut acc: i64 = 0;
    for i in 0 .. 5 {
        acc = acc + i as i64;
    }
    while acc < 20 { acc = acc + 1; }
    return;
}
```

---
## 9. Fases de Implementación
1. Scanner: nuevos tokens (`for`, `in`, `..`, tipos, operadores). 
2. Parser: incorporar `ForRangeStmt` y nueva jerarquía `Postfix`.
3. AST: nodos para `ForRangeStmt`, `CastExpr`, tipos, punteros.
4. Análisis semántico: resolver inferencia, insertar casts, validar rangos.
5. Codegen: soporte i32/i64/f32/f64, punteros, expansión de `for`.
6. Tests: casos de promoción numérica, cast descendente, for-range, punteros básicos, if expresión.

---
Este README actualizado es la referencia central para la implementación extendida.
