.data
.Lprint_fmt: .string "%ld\n"
.text
.globl calculate
calculate:
  pushq %rbp
  movq %rsp, %rbp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  subq $16, %rsp
  movq -8(%rbp), %rax
  # Cast to i64 (not implemented)
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  jmp .Lend_calculate
.Lend_calculate:
  leave
  ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $10, %rax
  movq %rax, -8(%rbp)
  movq $1000, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  movq -16(%rbp), %rax
  movq %rax, %rsi
  call calculate
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $5, %rax
  movq %rax, -32(%rbp)
  movq $10, %rax
  movq %rax, -40(%rbp)
  movq -32(%rbp), %rax
  # Cast to i64 (not implemented)
  pushq %rax
  movq -40(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -48(%rbp)
  # FloatExp 2.5 (not implemented)
  movq %rax, -56(%rbp)
  # FloatExp 3.7 (not implemented)
  movq %rax, -64(%rbp)
  movq -56(%rbp), %rax
  # Cast to f64 (not implemented)
  pushq %rax
  movq -64(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -72(%rbp)
  movq -72(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
