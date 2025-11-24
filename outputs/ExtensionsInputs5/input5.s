global main
extern printf
section .text
section .data
global_inferred: dq 999
section .text
convert_and_double:
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
  sub rsp, 8
  mov rax, [rbp-16]
  mov [rbp-32], rax
section .data
fmt0: db "Converted: %lld",10,0
section .text
  mov rax, [rbp-32]
  mov rsi, rax
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  sub rsp, 8
  mov rax, 10
  mov [rbp-40], rax
  sub rsp, 8
  mov rax, 20
  mov [rbp-48], rax
  sub rsp, 8
  mov rax, [rbp-40]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-48]
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-40]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-48]
  neg rax
  mov r10, [rbp-8]
  neg rax
  mov rcx, r10
  sub rcx, rax
  mov rax, rcx
  mov r10, [rbp-8]
  imul rax, r10
  mov [rbp-56], rax
  sub rsp, 8
  mov rax, 5
  mov [rbp-64], rax
  sub rsp, 8
  mov rax, [rbp-64]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov [rbp-72], rax
section .data
fmt1: db "%lld",10,0
section .text
  mov rax, [rbp-72]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
  add rsp, 8
  sub rsp, 8
  mov rax, 0
  mov [rbp-80], rax
  sub rsp, 8
  mov rax, 1
  mov [rbp-88], rax
L_for_begin2:
  mov rax, [rbp-88]
  push rax
  mov rax, 10
  pop r11
  cmp r11, rax
  jge L_for_end2
  mov rax, [rbp-80]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-88]
  mov r10, [rbp-8]
  add rax, r10
  mov [rbp-80], rax
  mov rax, [rbp-88]
  inc rax
  mov [rbp-88], rax
  jmp L_for_begin2
L_for_end2:
  sub rsp, 8
  mov rax, [rbp-80]
  cvtsi2sd xmm0, rax
  movq rax, xmm0
  mov r10, rax
  mov [rbp-8], r10
section .data
numFloat3: dq 2.0
section .text
  mov rax, [rel numFloat3]
  mov r10, [rbp-8]
  movq xmm0, r10
  movq xmm1, rax
  divsd xmm0, xmm1
  movq rax, xmm0
  mov [rbp-96], rax
section .data
fmt2: db "Final: %lld",10,0
section .text
  mov rax, [rbp-96]
  mov rsi, rax
  lea rdi, [rel fmt2]
  xor eax, eax
  call printf
L_func_end1:
  xor eax, eax
  add rsp, 96
  pop rbp
  ret
