; extern str_len
extern string_equals

section .text

global find_word
find_word:
  test rsi, rsi ; ? rsi = nullptr
  jz .found
  
  push rdi
  push rsi
  add rsi, 8 ; 8 = SIZEOF(dq)
  call string_equals
  pop rsi
  pop rdi
  
  test rax, rax
  jnz .found
  sub rsi, 8
  mov rsi, [rsi]
  jmp find_word

  .found:
    mov rax, rsi
    ret
