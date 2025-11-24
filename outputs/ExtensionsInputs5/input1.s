global main
extern printf
section .text
section .data
inferred_global: dq 42
section .text
section .data
inferred_float: dq 3.14
section .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, 10
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, 20
  mov [rbp-24], rax
  sub rsp, 8
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-32], rax
section .data
fmt0: db "Sum: %lld",10,0
section .text
  mov rax, [rbp-32]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  sub rsp, 8
  mov rax, 1
  mov [rbp-40], rax
  sub rsp, 8
  mov [rbp-48], rax
  mov rax, [rbp-40]
  cmp rax, 0
  je L_if_else1
  sub rsp, 8
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  imul rax, r10
  mov [rbp-56], rax
section .data
fmt1: db "%lld",10,0
section .text
  mov rax, [rbp-56]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
  add rsp, 8
  add rsp, 8
  jmp L_if_end1
L_if_else1:
L_if_end1:
L_func_end0:
  xor eax, eax
  add rsp, 48
  pop rbp
  ret
