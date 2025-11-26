.data
print_fmt: .string "%ld \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  # FloatExp 3.14 (simplified as int)
  movq $3, %rax
  movq %rax, -8(%rbp)
  # FloatExp 2.718 (simplified as int)
  movq $2, %rax
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  pushq %rax
  # FloatExp 10.5 (simplified as int)
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
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
