section .data
  msg: db 'Print OK!',10,0
section .text
  global _start
_start:
  mov rax,1
  mov rdi,1
  lea rsi,[msg]
  mov rdx,15
  syscall
  mov rax,60
  xor rdi,rdi
  syscall
