.data
.Lprint_fmt: .string "%ld\n"
inferred_global: .quad 0
inferred_float: .quad 0
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $10, %rax
  movq %rax, -8(%rbp)
  movq $20, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $1, %rax
  movq %rax, -32(%rbp)
  # StringExp "Rust" (not implemented)
  movq %rax, -40(%rbp)
  movq -32(%rbp), %rax
  cmpq $0, %rax
  je .Lelse_0
  movq -8(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -48(%rbp)
  movq -48(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  jmp .Lendif_0
.Lelse_0:
.Lendif_0:
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
