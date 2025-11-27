.data
print_fmt: .string "%ld \n"
.text
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq $2147483647, %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  movsxd %eax, %rax  # i32 → i64 (extensión de signo)
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $-1, %rax
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  # i64 → i32 (truncado implícito en %eax)
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq $100, %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  movsxd %eax, %rax  # i32 → i64 (extensión de signo)
  pushq %rax
  movq $1000000, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
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
