.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq $10, %rax
  movq %rax, -8(%rbp)
  # FloatExp 3.14
.section .rodata
.LC0:
  .double 3.14
.text
  movsd .LC0(%rip), %xmm0
  movsd %xmm0, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq $5, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm2
  # FloatExp 1.5
.section .rodata
.LC1:
  .double 1.5
.text
  movsd .LC1(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd %xmm2, %xmm0
  addsd %xmm1, %xmm0
  movsd %xmm0, -32(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movsd -32(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
