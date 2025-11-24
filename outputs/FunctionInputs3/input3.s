global main
extern printf
section .text
maximo:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  sub rsp, 8
  mov [rbp-24], rsi
  sub rsp, 8
  mov qword [rbp-32], 0
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  cmp r10, rax
  setg al
  movzx rax, al
  cmp rax, 0
  je L_if_else1
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-32], rax
  jmp L_if_end1
L_if_else1:
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-32], rax
L_if_end1:
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
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 15
  mov rdi, rax
  mov rax, 10
  mov rsi, rax
  call maximo
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-16], rax
section .data
fmt0: db "%lld",10,0
section .text
  mov rax, [rbp-16]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
L_func_end2:
  xor eax, eax
  add rsp, 16
  pop rbp
  ret
