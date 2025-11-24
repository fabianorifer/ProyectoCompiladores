global main
extern printf
section .text
section .data
x: dq 10
section .text
section .data
y: dq 20
section .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, [rel x]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 5
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-16], rax
section .data
fmt0: db "Resultado: %lld",10,0
section .text
  mov rax, [rbp-16]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  mov rax, [rel y]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rel y]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 1
  mov r10, [rbp-8]
  add rax, r10
  mov r10, [rbp-8]
  add rax, r10
  mov [rel y], rax
section .data
fmt1: db "%lld",10,0
section .text
  mov rax, [rel y]
  mov rsi, rax
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
L_func_end0:
  xor eax, eax
  add rsp, 16
  pop rbp
  ret
