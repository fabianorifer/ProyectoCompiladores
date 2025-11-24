.data
.Lprint_fmt: .string "%ld\n"
inferred: .quad 0
.text
.globl process
process:
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
  # Cast to f64 (not implemented)
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  pushq %rax
  # FloatExp 3 (not implemented)
  movq %rax, %rcx
  popq %rax
  cqto
  idivq %rcx
  jmp .Lend_process
.Lend_process:
  leave
  ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $50, %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  # Cast to i64 (not implemented)
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  movq %rax, %rdi
  call process
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $0, %rax
  movq %rax, -32(%rbp)
.Lfor_0:
  movq -32(%rbp), %rax
  pushq %rax
  movq $5, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  jge .Lendfor_0
  movq -32(%rbp), %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -40(%rbp)
  movq -40(%rbp), %rax
  # Cast to f32 (not implemented)
  movq %rax, -48(%rbp)
  movq -48(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -32(%rbp), %rax
  addq $1, %rax
  movq %rax, -32(%rbp)
  jmp .Lfor_0
.Lendfor_0:
  movq $10, %rax
  pushq %rax
  movq $5, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movl $0, %eax
  setg %al
  movq %rax, -56(%rbp)
  movq -56(%rbp), %rax
  cmpq $0, %rax
  je .Lelse_1
  # StringExp "Greater" (not implemented)
  movq %rax, -64(%rbp)
  movq -64(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  jmp .Lendif_1
.Lelse_1:
.Lendif_1:
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
