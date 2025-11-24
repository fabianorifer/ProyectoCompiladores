global main
extern printf
section .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, 100
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, [rbp-16]
  mov [rbp-24], rax
section .data
fmt0: db "Converted to i64: %lld",10,0
section .text
  mov rax, [rbp-24]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  add rsp, 8
  sub rsp, 8
section .data
numFloat1: dq 3.14
section .text
  mov rax, [rel numFloat1]
  mov [rbp-32], rax
  sub rsp, 8
  mov rax, [rbp-32]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov [rbp-40], rax
  sub rsp, 8
  mov rax, 42
  mov [rbp-48], rax
  sub rsp, 8
  mov rax, [rbp-48]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov [rbp-56], rax
section .data
fmt1: db "i32 to f32: %lld",10,0
section .text
  mov rax, [rbp-56]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
  add rsp, 8
  sub rsp, 8
  mov rax, [rbp-16]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov [rbp-64], rax
section .data
fmt2: db "Chained: %lld",10,0
section .text
  mov rax, [rbp-64]
  mov rsi, rax
  lea rdi, [rel fmt2]
  xor eax, eax
  call printf
L_func_end0:
  xor eax, eax
  add rsp, 64
  pop rbp
  ret
