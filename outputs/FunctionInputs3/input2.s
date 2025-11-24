.data
.Lprint_fmt: .string "%ld\n"
.text
.globl multiplicar
multiplicar:
  pushq %rbp
  movq %rsp, %rbp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  subq $16, %rsp
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  jmp .Lend_multiplicar
.Lend_multiplicar:
  leave
  ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $5, %rax
  movq %rax, -8(%rbp)
  movq $8, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  movq -16(%rbp), %rax
  movq %rax, %rsi
  call multiplicar
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
