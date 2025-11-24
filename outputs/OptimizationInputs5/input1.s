global main
extern printf
section .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  sub rsp, 8
  mov qword [rbp-16], 0
  mov rax, [rbp-16]
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 1
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 2
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 3
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 4
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 5
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 6
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 7
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 8
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 9
  mov r10, [rbp-8]
  add rax, r10
  mov r10, rax
  mov [rbp-8], r10
  mov rax, 10
  mov r10, [rbp-8]
  add rax, r10
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
L_func_end0:
  xor eax, eax
  add rsp, 16
  pop rbp
  ret
