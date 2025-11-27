.data
print_fmt: .string "%ld \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq $10, %rax
  movq %rax, -8(%rbp)
  leaq -8(%rbp), %rax
  # puntero/i64 → puntero (no-op, ya en %rax)
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  # puntero → i64 (no-op, ya en %rax)
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -24(%rbp), %rax
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
  movq $0, %rax
  jmp .end_main
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
