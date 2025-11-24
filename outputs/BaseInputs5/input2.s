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
  mov rax, 0
  mov [rbp-24], rax
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 0
  mov r10, [rbp-8]
  cmp r10, rax
  setg al
  movzx rax, al
  cmp rax, 0
  je L_if_else1
  mov rax, [rbp-24]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  imul rax, r10
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-24], rax
  jmp L_if_end1
L_if_else1:
  mov rax, [rbp-24]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  neg rax
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-24], rax
L_if_end1:
  mov rax, [rbp-24]
  jmp L_func_end0
L_func_end0:
  add rsp, 24
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, 0
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, 1
  mov [rbp-24], rax
L_for_begin3:
  mov rax, [rbp-24]
  push rax
  mov rax, 10
  pop r11
  cmp r11, rax
  jge L_for_end3
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-16], rax
  mov rax, [rbp-24]
  inc rax
  mov [rbp-24], rax
  jmp L_for_begin3
L_for_end3:
  sub rsp, 8
  mov rax, 0
  mov [rbp-32], rax
L_while_start4:
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 5
  mov r10, [rbp-8]
  cmp r10, rax
  setl al
  movzx rax, al
  cmp rax, 0
  je L_while_end4
section .data
fmt0: db "Counter: %lld",10,0
section .text
  mov rax, [rbp-32]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 1
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-32], rax
  jmp L_while_start4
L_while_end4:
section .data
fmt1: db "Sum: %lld",10,0
section .text
  mov rax, [rbp-16]
  mov rsi, rax
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
L_func_end2:
  xor eax, eax
  add rsp, 32
  pop rbp
  ret
