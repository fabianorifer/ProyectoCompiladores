.data
.Lprint_fmt: .string "%ld\n"
.text
.globl suma
suma:
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
  addq %rcx, %rax
  jmp .Lend_suma
.Lend_suma:
  leave
  ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  movq $1, %rax
  movq %rax, -8(%rbp)
  movq $20, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  movq -16(%rbp), %rax
  movq %rax, %rsi
  call suma
  movq %rax, %rsi
  leaq .Lprint_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
.Lend_main:
  leave
  ret
.section .note.GNU-stack,"",@progbits
