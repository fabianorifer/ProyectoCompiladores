.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl maximo
maximo:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movl $0, %eax
  setg %al
  cmpq $0, %rax
  je .else_0
  movq -8(%rbp), %rax
  movq %rax, -24(%rbp)
  jmp .endif_0
.else_0:
  movq -16(%rbp), %rax
  movq %rax, -24(%rbp)
.endif_0:
  movq -24(%rbp), %rax
  jmp .end_maximo
  movq $0, %rax
  jmp .end_maximo
.end_maximo:
 leave
 ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq $15, %rax
  movq %rax, %rdi
  movq $10, %rax
  movq %rax, %rsi
  movl $0, %eax
  call maximo
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
