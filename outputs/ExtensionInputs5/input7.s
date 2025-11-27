.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $120, %rsp
  movq $10, %rax
  movq %rax, -8(%rbp)
  leaq -8(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -32(%rbp)
  movq $20, %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movl %eax, (%rcx)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -32(%rbp), %rax
  movl (%rax), %eax
  movq %rax, -64(%rbp)
  movq -64(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -32(%rbp), %rax
  # puntero → i64 (no-op, ya en %rax)
  movq %rax, -56(%rbp)
  movq -56(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -56(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -72(%rbp)
  movq $30, %rax
  pushq %rax
  movq -72(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movl %eax, (%rcx)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $1000000, %rax
  movq %rax, -16(%rbp)
  leaq -16(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -40(%rbp)
  movq $2000000, %rax
  pushq %rax
  movq -40(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movq %rax, (%rcx)
  movq -16(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  # FloatExp 3.14159
.section .rodata
.LC0:
  .double 3.14159
.text
  movsd .LC0(%rip), %xmm0
  movsd %xmm0, -24(%rbp)
  leaq -24(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -48(%rbp)
  movsd -24(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  # FloatExp 2.71828
.section .rodata
.LC1:
  .double 2.71828
.text
  movsd .LC1(%rip), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
  movq -48(%rbp), %rax
  movq %rax, %rcx
  movsd (%rsp), %xmm0
  addq $8, %rsp
  movsd %xmm0, (%rcx)
  movsd -24(%rbp), %xmm0
  leaq float_fmt(%rip), %rdi
  movl $1, %eax  # 1 argumento XMM
  call printf@PLT
  movq $100, %rax
  movq %rax, -8(%rbp)
  movq -32(%rbp), %rax
  movl (%rax), %eax
  pushq %rax
  movq $50, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movl %eax, (%rcx)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  leaq -8(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -80(%rbp)
  leaq -8(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -88(%rbp)
  movq $200, %rax
  pushq %rax
  movq -80(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movl %eax, (%rcx)
  movq -88(%rbp), %rax
  movl (%rax), %eax
  pushq %rax
  movq $100, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -88(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movl %eax, (%rcx)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -32(%rbp), %rax
  # puntero → i64 (no-op, ya en %rax)
  movq %rax, -96(%rbp)
  movq -96(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -104(%rbp)
  movq $500, %rax
  pushq %rax
  movq -104(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movl %eax, (%rcx)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  leaq -8(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -112(%rbp)
  movq -112(%rbp), %rax
  movl (%rax), %eax
  movq %rax, -120(%rbp)
  movq -120(%rbp), %rax
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
