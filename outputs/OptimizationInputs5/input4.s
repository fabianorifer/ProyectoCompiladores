.data
print_fmt: .string "%ld \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movq $0, %rax
  cmpq $0, %rax
  je .else_0
  movq $100, %rax
  movq %rax, -8(%rbp)
  jmp .endif_0
.else_0:
  movq $5, %rax
  movq %rax, -8(%rbp)
.endif_0:
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
