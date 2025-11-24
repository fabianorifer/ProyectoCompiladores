global main
extern printf
section .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, 42
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, [rbp-16]
  lea rax, [rbp-16]
  mov [rbp-24], rax
  mov rax, [rbp-24]
  mov rax, [rax]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 100
  mov r10, [rbp-8]
  add rax, r10
  mov rax, [rbp-24]
  push rax
  mov rax, [rbp-24]
  mov rax, [rax]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 100
  mov r10, [rbp-8]
  add rax, r10
  pop rcx
  mov [rcx], rax
section .data
fmt0: db "Value: %lld",10,0
section .text
  mov rax, [rbp-16]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  add rsp, 8
  sub rsp, 8
  mov rax, 999
  mov [rbp-32], rax
  sub rsp, 8
  mov rax, [rbp-32]
  lea rax, [rbp-32]
  mov [rbp-40], rax
section .data
fmt1: db "%lld",10,0
section .text
  mov rax, [rbp-40]
  mov rax, [rax]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
  add rsp, 8
L_func_end0:
  xor eax, eax
  add rsp, 40
  pop rbp
  ret
