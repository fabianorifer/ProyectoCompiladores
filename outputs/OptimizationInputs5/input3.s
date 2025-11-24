.data
.Lprint_fmt: .string "%ld\n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $2, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq $6, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movl $0, %eax
  setl %al
  cmpq $0, %rax
  je .Lelse_0
  movq $1, %rax
  movq %rax, -8(%rbp)
  jmp .Lendif_0
.Lelse_0:
  movq $2, %rax
  movq %rax, -8(%rbp)
.Lendif_0:
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
