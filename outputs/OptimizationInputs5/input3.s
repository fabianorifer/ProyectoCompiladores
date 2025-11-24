global main
extern printf
section .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov qword [rbp-16], 0
  mov rax, 2
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 6
  mov r10, [rbp-8]
  cmp r10, rax
  setl al
  movzx rax, al
  cmp rax, 0
  je L_if_else1
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 1
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-16], rax
  jmp L_if_end1
L_if_else1:
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-16], rax
L_if_end1:
section .data
fmt0: db "%lld",10,0
section .text
  mov rax, [rbp-16]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
L_func_end0:
  xor eax, eax
  add rsp, 16
  pop rbp
  ret
