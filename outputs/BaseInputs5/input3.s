global main
extern printf
section .text
section .data
pi: dq 3.14159
section .text
section .data
e: dq 2.71828
section .text
compute_area:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov [rbp-16], rdi
  sub rsp, 8
  mov rax, [rel pi]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  mov rcx, rax
  mov rax, r10
  mov rdx, 1
  cmp rcx, 0
  je L_pow_end1
L_pow_loop1:
  imul rdx, rax
  dec rcx
  jne L_pow_loop1
L_pow_end1:
  mov rax, rdx
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
section .data
numFloat3: dq 5.0
section .text
  mov rax, [rel numFloat3]
  mov [rbp-16], rax
  sub rsp, 8
  mov rax, [rbp-16]
  mov rdi, rax
  sub rsp, 8
  call compute_area
  add rsp, 8
  mov [rbp-24], rax
section .data
fmt0: db "Area: %lld",10,0
section .text
  mov rax, [rbp-24]
  mov rsi, rax
  sub rsp, 8
  lea rdi, [rel fmt0]
  xor eax, eax
  call printf
  add rsp, 8
  sub rsp, 8
section .data
numFloat4: dq 1.5e10
section .text
  mov rax, [rel numFloat4]
  mov [rbp-32], rax
section .data
fmt1: db "%lld",10,0
section .text
  mov rax, [rbp-32]
  mov rsi, rax
  lea rdi, [rel fmt1]
  xor eax, eax
  call printf
L_func_end2:
  xor eax, eax
  add rsp, 32
  pop rbp
  ret
