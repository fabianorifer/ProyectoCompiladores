.data
print_fmt: .string "%ld \n"
float_fmt: .string "%f \n"
.text
.globl suma
suma:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  jmp .end_suma
  movq $0, %rax
  jmp .end_suma
.end_suma:
 leave
 ret
.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq $8, %rax
  movq %rax, -8(%rbp)
  movq $12, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  movq -16(%rbp), %rax
  movq %rax, %rsi
  movl $0, %eax
  call suma
  movq %rax, -24(%rbp)
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
