; extern exit
; extern print_uint

section .data
bmp_file_name: db "my_bmp", 0
sockaddr:
  .sa_family: dw 2 ; AF_INET
  .in_port: dw 0x9026 ; выбрал случайный (9872)
  .in_addr: db 127, 0, 0, 1 ; localhost
  .in_zero: dq 0

section .text
global _start
_start:
  mov rax, 2 ; open syscall №
  mov rdi, bmp_file_name
  mov rsi, 0
  syscall

  cmp eax, 0
  jl .fail

.get_first_six_bytes:
  push rax ; rax = fd
  mov rdi, rax ; rdi = fd
  xor rax, rax ; read syscall №

  push word 0
  push word 0
  push word 0

  mov rsi, rsp
  mov rdx, 6
  syscall

  pop di
  pop ax
  xor rsi, rsi ; понадобится дальше: старшие четыре байта должны быть равны 0
  pop si
  
.is_bmp:
  cmp di, "BM"
  jne .fail
  
.store_file_size_in_rsi:
  shl esi, 2 * 8
  mov si, ax

.call_mmap:
  mov rax, 9 ; mmap syscall №
  xor rdi, rdi ; rdi = nullptr
  mov rdx, 1 ; PROT_READ
  mov r10, 2 ; MAP_PRIVATE
  pop r8 ; r8 = fd
  xor r9, r9 ; offset = 0
  push rsi ; размер файла
  syscall
  pop rcx ; размер файла сохраним в rcx
  
  cmp eax, -1
  je .fail
  
.new_socket:
  mov rdi, 2 ; AF_INET
  mov rsi, 1 ; SOCK_STREAM
  xor rdx, rdx ; protocol = 0
  
  push rax ; адрес начала отображения
  mov rax, 41 ; socket syscall №
  push rcx ; размер файла
  syscall
  pop rdx ; размер файла храним в rdx
  pop rsi ; rsi = (адрес начала отображения)
  
  cmp eax, -1
  je .fail
  
  push rdx ; сохраняем размер файла
  push rsi ; сохраняем адрес начала отображения

.establish_connection:
  mov rdi, rax
  mov rsi, sockaddr
  mov rdx, 16 ; длина sockaddr
  mov rax, 42 ; connect syscall №
  push rdi ; сохраняем sockfd
  syscall
  pop rdi ; храним sockfd в rdi
  pop rsi ; rsi = (адрес начала отображения)
  pop rdx ; храним, сколько байт ещё не передали, в rdx
  
  test rax, rax
  jne .fail
  
.send_file:
;  .loop:
    
    mov rax, 44 ; sendto syscall №
    xor r10, r10 ; flags = 0
    xor r8, r8 ; destAddr = NULL
    xor r9, r9; addrLen = 0
    jmp .send_last_packet
;    cmp rdx, 1024
    ; jbe .send_last_packet
    
;  .send_next_packet:
;    sub rdx, 1024
;    push rdx
;    mov rdx, 1024
;    push rsi
;    push rdi
;    syscall
;    pop rdi
;    pop rsi
;    add rsi, 1024
;    pop rdx
;    cmp rax, 1024
;    jne .fail
;    mov rax, rdi
    
    ; push rdx
    ; push rsi
    ; jmp .establish_connection
    
;    jmp .loop
    
  .send_last_packet:
    push rdx
    syscall
    pop rdx
    cmp rax, rdx
    jne .fail
  
.ok:
  xor rdi, rdi
  mov rax, 60
  syscall
  

.fail:
  ; xor rdi, rdi
  ; mov edi, eax
  ; neg edi
  mov rdi, 1
  mov rax, 60
  syscall
