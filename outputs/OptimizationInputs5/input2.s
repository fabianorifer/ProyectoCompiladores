.data
.Lprint_fmt: .string "%ld\n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $10, %rax
  pushq %rax
  movq $5, %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $3, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -8(%rbp)
  movq $4, %rax
  pushq %rax
  movq $6, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
