.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  # FloatExp 3.14159
.section .rodata
.LC0:
  .double 3.14159
.text
  movsd .LC0(%rip), %xmm0
  movsd %xmm0, -8(%rbp)
  movsd -8(%rbp), %xmm0
  # f64 → f64 (sin cambio)
  movsd %xmm0, -16(%rbp)
  movsd -16(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  # FloatExp 2.71828
.section .rodata
.LC1:
  .double 2.71828
.text
  movsd .LC1(%rip), %xmm0
  movsd %xmm0, -16(%rbp)
  movsd -16(%rbp), %xmm0
  cvtsd2ss %xmm0, %xmm0  # f64 → f32
  movsd %xmm0, -24(%rbp)
  movsd -24(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  movsd -8(%rbp), %xmm0
  # f64 → f64 (sin cambio)
  movsd %xmm0, %xmm2
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm1
  movsd %xmm2, %xmm0
  addsd %xmm1, %xmm0
  movsd %xmm0, -32(%rbp)
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
