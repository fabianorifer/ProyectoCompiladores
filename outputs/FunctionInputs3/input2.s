global main
extern printf
section .text
multiplicar:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  sub rsp, 8
  mov [rbp-24], rsi
  sub rsp, 8
  mov qword [rbp-32], 0
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  imul rax, r10
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-32], rax
  mov rax, [rbp-32]
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
  mov qword [rbp-16], 0
  sub rsp, 8
  mov qword [rbp-24], 0
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 5
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-16], rax
  mov rax, [rbp-24]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 8
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-24], rax
section .data
fmt0: db "%lld",10,0
section .text
  mov rax, [rbp-16]
  mov rdi, rax
  mov rax, [rbp-24]
  mov rsi, rax
  sub rsp, 8
  call multiplicar
  add rsp, 8
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  add rsp, 8
L_func_end1:
  xor eax, eax
  add rsp, 24
  pop rbp
  ret
