.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  # FloatExp 3.14
.section .rodata
.LC0:
  .double 3.14
.text
  movsd .LC0(%rip), %xmm0
  movsd %xmm0, -8(%rbp)
  movsd -8(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
