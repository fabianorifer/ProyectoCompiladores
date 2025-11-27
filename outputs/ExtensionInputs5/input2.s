.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq $5, %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  cvtsi2sd %rax, %xmm0  # i64 → f64
  movsd %xmm0, %xmm2
  # FloatExp 2.5
.section .rodata
.LC0:
  .double 2.5
.text
  movsd .LC0(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd %xmm2, %xmm0
  addsd %xmm1, %xmm0
  movsd %xmm0, -16(%rbp)
  movsd -16(%rbp), %xmm0
  cvttsd2si %xmm0, %rax  # f64 → i64 (trunca)
  movq %rax, -24(%rbp)
  movsd -16(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
