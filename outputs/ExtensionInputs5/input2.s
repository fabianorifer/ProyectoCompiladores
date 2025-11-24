.data
.Lprint_fmt: .string "%ld\n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $100, %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  # Cast to i64 (not implemented)
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  # FloatExp 3.14 (not implemented)
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  # Cast to f64 (not implemented)
  movq %rax, -32(%rbp)
  movq $42, %rax
  movq %rax, -40(%rbp)
  movq -40(%rbp), %rax
  # Cast to f32 (not implemented)
  movq %rax, -48(%rbp)
  movq -48(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -8(%rbp), %rax
  # Cast to i64 (not implemented)
  # Cast to f64 (not implemented)
  movq %rax, -56(%rbp)
  movq -56(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
