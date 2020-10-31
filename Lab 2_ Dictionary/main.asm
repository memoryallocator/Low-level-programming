%include "colon.inc"
%include "words.inc"

extern exit
extern string_length
extern print_string
extern print_newline

extern find_word


section .data

str_error_key_is_too_long: db "Error: key is too long", 0
str_error_key_not_found: db "Error: key not found", 0

section .text

global _start
_start:
  %rep 32
  push qword 0 ; 8 * 32 = 256
  %endrep
  
  xor rax, rax
  mov rdi, 0
  mov rsi, rsp
  mov rdx, 256
  syscall
  
  cmp [rsp+255], byte 0
  je .input_string_length_is_ok
  
  cmp [rsp+255], byte 10
  je .input_string_length_is_ok
  
  mov rdi, str_error_key_is_too_long
  call print_string
  jmp .fail
  
  .input_string_length_is_ok:
    .preprocess_input:
      mov rdi, rsp
      call string_length
      mov [rsp+rax-1], byte 0 ; '\n' -> '\0'
  
  .input_string_is_ok:
    mov rdi, rsp
    mov rsi, last_entry
    call find_word
    test rax, rax
    jnz .ok
    
    mov rdi, str_error_key_not_found
    call print_string
    jmp .fail

.fail:
  call print_newline
  mov rdi, 1
  call exit

.ok:
  add rax, 8
  mov rdi, rax
  push rdi
  call string_length
  pop rdi
  inc rax
  add rdi, rax
  call print_string
  call print_newline
  xor rdi, rdi
  call exit
