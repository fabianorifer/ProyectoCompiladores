global main
extern printf
section .text
section .data
global_flag: dq 1
section .text
section .data
counter: dq 0
section .text
factorial:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 1
  mov r10, [rbp-8]
  cmp r10, rax
  setle al
  movzx rax, al
  cmp rax, 0
  je L_if_else1
  mov rax, 1
  jmp L_func_end0
  jmp L_if_end1
L_if_else1:
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 1
  neg rax
  mov r10, [rbp-8]
  neg rax
  mov rcx, r10
  sub rcx, rax
  mov rax, rcx
  mov rdi, rax
  call factorial
  mov r10, [rbp-8]
  imul rax, r10
  jmp L_func_end0
L_if_end1:
L_func_end0:
  add rsp, 16
  pop rbp
  ret
process:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  sub rsp, 8
  mov [rbp-24], rsi
  sub rsp, 8
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  neg rax
  mov r10, [rbp-8]
  neg rax
  mov rcx, r10
  sub rcx, rax
  mov rax, rcx
  mov r10, [rbp-8]
  imul rax, r10
  mov [rbp-32], rax
  mov rax, [rbp-32]
  jmp L_func_end2
L_func_end2:
  add rsp, 32
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov rax, 5
  mov rdi, rax
  call factorial
  mov [rbp-16], rax
section .data
fmt0: db "Factorial: %lld",10,0
section .text
  mov rax, [rbp-16]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  sub rsp, 8
  mov rax, 0
  mov [rbp-24], rax
  sub rsp, 8
  mov rax, 0
  mov [rbp-32], rax
L_for_begin4:
  mov rax, [rbp-32]
  push rax
  mov rax, 10
  pop r11
  cmp r11, rax
  jge L_for_end4
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  mov r11, rax
  mov rax, r10
  cqo
  idiv r11
  mov rax, rdx
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 0
  mov r10, [rbp-8]
  cmp r10, rax
  sete al
  movzx rax, al
  cmp rax, 0
  je L_if_else5
  mov rax, [rbp-24]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-32]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-24], rax
  jmp L_if_end5
L_if_else5:
L_if_end5:
  mov rax, [rbp-32]
  inc rax
  mov [rbp-32], rax
  jmp L_for_begin4
L_for_end4:
  sub rsp, 8
  mov rax, 10
  mov rdi, rax
  mov rax, 3
  mov rsi, rax
  sub rsp, 8
  call process
  add rsp, 8
  mov [rbp-40], rax
section .data
fmt1: db "Processed: %lld",10,0
section .text
  mov rax, [rbp-40]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
  add rsp, 8
  mov rax, [rel global_flag]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-24]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 5
  mov r10, [rbp-8]
  cmp r10, rax
  setg al
  movzx rax, al
  mov r10, [rbp-8]
  cmp r10, 0
  je L_and_false7
  cmp rax, 0
  je L_and_false7
  mov rax, 1
  jmp L_and_end7
L_and_false7:
  xor rax, rax
L_and_end7:
  cmp rax, 0
  je L_if_else6
section .data
fmt2: db "Condition met",10,0
section .text
  sub rsp, 8
  lea rdi, [rel fmt2]
  xor eax, eax
  call printf
  add rsp, 8
  jmp L_if_end6
L_if_else6:
L_if_end6:
L_func_end3:
  xor eax, eax
  add rsp, 40
  pop rbp
  ret
