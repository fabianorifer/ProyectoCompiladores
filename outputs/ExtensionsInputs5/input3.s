global main
extern printf
section .text
calculate:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  sub rsp, 8
  mov [rbp-24], rsi
  sub rsp, 8
  mov rax, [rbp-16]
  mov [rbp-32], rax
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  add rax, r10
  jmp L_func_end0
L_func_end0:
  add rsp, 32
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, 10
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, 1000
  mov [rbp-24], rax
  sub rsp, 8
  mov rax, [rbp-16]
  mov rdi, rax
  mov rax, [rbp-24]
  mov rsi, rax
  call calculate
  mov [rbp-32], rax
section .data
fmt0: db "Result: %lld",10,0
section .text
  mov rax, [rbp-32]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  sub rsp, 8
  mov rax, 5
  mov [rbp-40], rax
  sub rsp, 8
  mov rax, 10
  mov [rbp-48], rax
  sub rsp, 8
  mov rax, [rbp-40]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-48]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-56], rax
  sub rsp, 8
section .data
numFloat2: dq 2.5
section .text
  mov rax, [rel numFloat2]
  mov [rbp-64], rax
  sub rsp, 8
section .data
numFloat3: dq 3.7
section .text
  mov rax, [rel numFloat3]
  mov [rbp-72], rax
  sub rsp, 8
  mov rax, [rbp-64]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-72]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-80], rax
section .data
fmt1: db "%lld",10,0
section .text
  mov rax, [rbp-80]
  mov rsi, rax
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
L_func_end1:
  xor eax, eax
  add rsp, 80
  pop rbp
  ret
