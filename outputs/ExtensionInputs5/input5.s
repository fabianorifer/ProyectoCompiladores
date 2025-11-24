.data
.Lprint_fmt: .string "%ld\n"
global_inferred: .quad 0
.text
.globl convert_and_double
convert_and_double:
  pushq %rbp
  movq %rsp, %rbp
  movq %rdi, -8(%rbp)
  subq $8, %rsp
  movq -8(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  # Cast to i64 (not implemented)
  jmp .Lend_convert_and_double
.Lend_convert_and_double:
  leave
  ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $42, %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  # REF not implemented
  movq %rax, -16(%rbp)
  movq $100, %rax
  movq -8(%rbp), %rax
  # Cast to i64 (not implemented)
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $10, %rax
  movq %rax, -32(%rbp)
  movq $20, %rax
  movq %rax, -40(%rbp)
  movq -32(%rbp), %rax
  pushq %rax
  movq -40(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -32(%rbp), %rax
  pushq %rax
  movq -40(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -48(%rbp)
  movq $5, %rax
  movq %rax, -56(%rbp)
  movq -56(%rbp), %rax
  # Cast to i64 (not implemented)
  # Cast to f64 (not implemented)
  movq %rax, -64(%rbp)
  movq -64(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $0, %rax
  movq %rax, -72(%rbp)
  movq $1, %rax
  movq %rax, -80(%rbp)
.Lfor_0:
  movq -80(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  jge .Lendfor_0
  movq -80(%rbp), %rax
  movq %rax, %rcx
  movq -72(%rbp), %rax
  addq %rcx, %rax
  movq %rax, -72(%rbp)
  movq -80(%rbp), %rax
  addq $1, %rax
  movq %rax, -80(%rbp)
  jmp .Lfor_0
.Lendfor_0:
  movq -72(%rbp), %rax
  # Cast to f32 (not implemented)
  pushq %rax
  # FloatExp 2 (not implemented)
  movq %rax, %rcx
  popq %rax
  cqto
  idivq %rcx
  movq %rax, -88(%rbp)
  movq -88(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
