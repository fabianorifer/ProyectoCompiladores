global main
extern printf
section .text
section .data
inferred: dq 100
section .text
process:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  sub rsp, 8
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  imul rax, r10
  mov [rbp-24], rax
  sub rsp, 8
  mov rax, [rbp-24]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov [rbp-32], rax
  mov rax, [rbp-32]
  mov r10, rax
  mov [rbp-8], r10
section .data
numFloat1: dq 3.0
section .text
  mov rax, [rel numFloat1]
  mov r10, [rbp-8]
  movq xmm0, r10
  movq xmm1, rax
  divsd xmm0, xmm1
  movq rax, xmm0
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
  mov rax, 50
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, [rbp-16]
  mov [rbp-24], rax
  sub rsp, 8
  mov rax, [rbp-24]
  mov rdi, rax
  call process
  mov [rbp-32], rax
section .data
fmt0: db "Output: %lld",10,0
section .text
  mov rax, [rbp-32]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  sub rsp, 8
  mov rax, 0
  mov [rbp-40], rax
L_for_begin3:
  mov rax, [rbp-40]
  push rax
  mov rax, 5
  pop r11
  cmp r11, rax
  jge L_for_end3
  sub rsp, 8
  mov rax, [rbp-40]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-40]
  mov r10, [rbp-8]
  imul rax, r10
  mov [rbp-48], rax
  sub rsp, 8
  mov rax, [rbp-48]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
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
  add rsp, 16
  mov rax, [rbp-40]
  inc rax
  mov [rbp-40], rax
  jmp L_for_begin3
L_for_end3:
  sub rsp, 8
  mov rax, 10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 5
  mov r10, [rbp-8]
  cmp r10, rax
  setg al
  movzx rax, al
  mov [rbp-48], rax
  mov rax, [rbp-48]
  cmp rax, 0
  je L_if_else4
  sub rsp, 8
  mov [rbp-56], rax
section .data
fmt2: db "%lld",10,0
section .text
  mov rax, [rbp-56]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt2]
  xor eax, eax
  call printf
  add rsp, 8
  add rsp, 8
  jmp L_if_end4
L_if_else4:
L_if_end4:
L_func_end2:
  xor eax, eax
  add rsp, 48
  pop rbp
  ret
