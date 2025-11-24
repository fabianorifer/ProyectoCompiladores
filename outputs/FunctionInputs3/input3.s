.data
.Lprint_fmt: .string "%ld\n"
.text
.globl maximo
maximo:
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
  cmpq %rcx, %rax
  movl $0, %eax
  setg %al
  cmpq $0, %rax
  je .Lelse_0
  movq -8(%rbp), %rax
  movq %rax, -24(%rbp)
  jmp .Lendif_0
.Lelse_0:
  movq -16(%rbp), %rax
  movq %rax, -24(%rbp)
.Lendif_0:
  movq -24(%rbp), %rax
  jmp .Lend_maximo
.Lend_maximo:
  leave
  ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $15, %rax
  movq %rax, %rdi
  movq $10, %rax
  movq %rax, %rsi
  call maximo
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
