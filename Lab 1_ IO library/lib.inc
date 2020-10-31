section .data

min_signed_x64_as_string:    db "-9223372036854775808", 0
max_signed_x64_as_string:    db "9223372036854775807", 0
max_unsigned_x64_as_string:  db "18446744073709551615", 0

section .text
 
global exit
; Принимает код возврата и завершает текущий процесс
exit:
    mov rax, 60
    syscall

global string_length
; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rcx, rcx ; зануляем rcx
    
    .loop:
        cmp byte[rdi+rcx], 0 ; нашли нуль-терминатор?
        je .done ; уходим, если нашли
        inc rcx ; смотрим дальше, если не нашли
        jmp .loop
        
    .done:
        mov rax, rcx ; ответ будем возвращать через регистр rax
        ret

global print_string
; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length ; после вызова этой функции в rax будет храниться длина строки
    pop rsi ; сохраним в rsi наш указатель на начало строки
    mov rdx, rax ; сколько байтов выводим (длина строки)
    mov rax, 1 ; номер системного вызова write
    mov rdi, 1 ; дескриптор stdout
    syscall
    ret

global print_newline
; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov di, 0xA ; код символа '\n'
    
global print_char
; Принимает код символа и выводит его в stdout
print_char:
    push di ; положим символ на вершину стека
    lea rsi, [rsp] ; сохраним в rsi указатель на символ
    mov rdx, 1 ; выводим один байт
    mov rax, 1 ; номер системного вызова write
    mov rdi, 1 ; дескриптор stdout
    syscall
    pop di ; возвращаем rsp на место
    ret

global print_uint
; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII-коды.
print_uint:
    mov r8, rsp
    push word 0 ; нуль-терминатор (может и не понадобиться)
    xor cl, cl
    xor r11w, r11w ; будет играть роль аккумулятора
    mov rax, rdi
    
    .loop:
        ; xor r9, r9
        mov r9, 10
        xor rdx, rdx
        div r9 ; rax = rax / 10, rdx = rax % 10
        mov r10, rax ; сохраняем значение rax
        
        mov al, dl
        add al, '0' ; al теперь содержит ASCII-код цифры
        
        test cl, cl
        jnz .write_ascii_code_to_low_byte_and_push
        
        .save_ascii_code:
            mov r11b, al
            inc cl
            jmp .ascii_code_has_been_processed
            
        .write_ascii_code_to_low_byte_and_push:
            mov r9b, al ; сохраним код текущего символа
            mov al, r11b ; младший байт теперь содержит код предыдущего символа
            mov ah, al ; также можно shl ax, 8
            mov al, r9b
            xor r11b, r11b
            xor cl, cl
            push ax
            
        .ascii_code_has_been_processed:
            test r10, r10 ; если r10 = (rax после деления) = 0, проверить,
            ; сформировали ли целое слово (2 байта), и вывести
            jz .check_whether_ax_is_full_or_not
            mov rax, r10 ; восстанавливаем значение rax
            jmp .loop
        
    .check_whether_ax_is_full_or_not:
        test cl, cl ; 0, если ax уже на стеке; 1, если к пушу готов только ah = r11b
        jz .stack_is_ready
        shl r11w, 8
        push r11w
        add rsp, 1
        
    .stack_is_ready:
        mov rax, rsp
        push rdi
        push r8
        mov rdi, rax
        call print_string
        pop r8
        pop rdi
    
    mov rsp, r8 ; вернули стек в исходное состояние
    ret

global print_int
; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    ; mov rdx, rax
    mov rax, 0x8000_0000_0000_0000
    cmp rdi, rax
    je .corner_case
    
    mov rcx, rdi
    and rcx, rax
    test rcx, rcx
    jz .argument_is_positive
    
    .argument_is_negative:
        push rdi
        mov di, '-'
        call print_char
        pop rdi
        neg rdi
    
    .argument_is_positive:
        ; mov rcx, 0x8000_0000_0000_0000
        ; not rcx
        ; and rdi, rcx
        call print_uint
        ret
        
    .corner_case:
        mov rdi, min_signed_x64_as_string
        call print_string
        ret


; Принимает в rdi, rsi указатели на начало строк
; Возвращает -1, если строка, начинающаяся в rax, меньше строки с началом в rdx
; 1, если больше, 0 — если они равны
; A > B, если | A | > | B | или | A | = | B | и существует i: a_i > b_i,
; а для любого j < i a_j = b_j, где | . | — длина строки, s_i — i-й символ строки s
; A < B определяется двойственно
; A = B, если | A | = | B | и a_i = b_i для любого i
str_cmp:
    cmp rdi, rsi
    je .eq
    
    xor r10b, r10b ; если есть i: a_i =/= b_i, то r10b = [1, если a_i < b_i, иначе 0]
    xor rcx, rcx
    .loop:
        mov r8b, byte[rdi + rcx]
        mov r9b, byte[rsi + rcx]
        test r8b, r8b ; закончилась ли первая строка
        jz .less_or_eq
        
        test r9b, r9b ; первая строка ещё не закочилась. А вторая?
        jz .greater ; если закончилась, то A > B
        
        inc rcx
        
        cmp r8b, r9b ; теперь сравниваем посимвольно
        je .loop ; если текущие символы равны, идём на следующую итерацию
        adc r10b, 0 ; если r8 < r9, (carry flag) = 1

    .cmp_len:
        add rdi, rcx ; rdi = указатель на оставшиеся символы
        add rsi, rcx
        
        push r10w
        push rdi
        
        push rsi
        call string_length
        mov rcx, rax ; в rcx — длина оставшихся символов первой строки
        pop rsi
        
        mov rdi, rsi
        
        push rcx
        call string_length
        pop rcx
        
        pop rdi        
        pop r10w
        
        cmp rax, rcx ; | B | ? | A |
        ja .less ; | B | > | A | <=> | A | < | B |
        jb .greater ; аналогично
        
        ; если мы здесь, то известно, что | A | = | B |
        test r10b, r10b ; если r10b = 0, то a_i > b_i,
        jz .greater ; а раз длины равны, то и A > B
        jmp .less

    .less_or_eq: ; если мы здесь — первая строка закончилась
        test r9b, r9b ; закончилась ли вторая строка
        jz .eq
    
    .less:
        mov rax, -1
        ret
        
    .eq:
        xor rax, rax
        ret
        
    .greater:
        mov rax, 1
        ret
        
global string_equals
; Принимает два указателя на нуль-терминированные строки, возвращает 1, если они равны, 0 иначе
string_equals:
    call str_cmp
    test rax, rax
    jz .eq

    .not_eq:
        xor rax, rax
        ret
          
    .eq:
        inc rax
        ret

global read_char
; Читает один символ из stdin и возвращает его. Возвращает 0, если достигнут конец потока
read_char:
    push word 0
    mov rax, 0
    mov rdi, 0
    lea rsi, [rsp]
    mov rdx, 1
    syscall
    pop ax
    cmp ax, 4 ; конец передачи (EOT)?
    jne .ok
    xor rax, rax
    
    .ok:
        ret

global read_word
; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы — это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0, если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
read_word:
    test rdx, rdx ; если размер буфера = 0,
    jz .base_case ; выходим
    mov rcx, 0 ; сколько символов прочитано 
    mov rdi, rax ; в rdi будем хранить адрес начала
    mov r8b, 0 ; когда пропустим пробельные символы в начале, установим равным 1
    .loop:
        push rdi
        push rcx
        push rdx
        push r8w
        call read_char
        pop r8w
        pop rdx
        pop rcx
        pop rdi
        test ax, ax ; конец потока?
        jz .write_null_terminator_and_return
        
        cmp al, 0x9 ; код символа '\t'
        je .skip_spacing_or_end
        
        cmp al, ' '
        je .skip_spacing_or_end ; пробельные символы в начале пропускаем
        
        cmp al, 0xA ; '\n'
        jne .curr_char_is_not_spacing
        .skip_spacing_or_end:
            test r8b, r8b ; если всё ещё пропускаем пробельные символы
            jz .loop
            jmp .write_null_terminator_and_return ; встретили пробельный символ не в начале
            
        .curr_char_is_not_spacing:
            mov r8b, 1 ; добрались до непробельного символа
            inc rcx ; прочитали символ, который нужно записать — увеличим rcx на 1
            cmp rcx, rdx ; помещается в буфер?
            ja .smth_went_wrong ; если rcx > rdx, строка слишком длинная.
            ; ситуация rcx = 1, rdx = 0 невозможна (этот случай обработан в начале)
            
            .write_al_to_buf:
                mov [rdi+rcx-1], al ; rcx — кол-во прочитанных байт
                ; значит, offset будет rcx-1 (если прочитали 1 байт, пишем в адрес буфера)
                
            test al, al ; если al = '\0'
            jnz .loop
            
            dec rcx ; так как мы записали '\0' как обычный символ, отнимем 1
            mov rax, rdi ; адрес начала буфера вернём через rax
            mov rdx, rcx ; длину прочитанного слова пишем в rdx
            ret
            
    .write_null_terminator_and_return:
        mov al, 0
        jmp .curr_char_is_not_spacing ; запишем как обычный символ
        
    .smth_went_wrong:
        xor rax, rax
        
    .base_case:
        ret
         
; Принимает указатель на строку в rsi
; Возвращает количество цифр в начале строки через rax
count_digits_at_the_beginning:
    xor rax, rax
    
    .loop:
        mov rdx, [rsi+rax] ; текущий символ
        cmp rdx, '0'
        jb .done
        
        cmp rdx, '9'
        ja .done
        
        inc rax
        jmp .loop
    
    .done:
        ret

; Принимает в rax, rdx указатели на начало строк, в rcx — их длины
; Сравнивает посимвольно строку [rax, rax + rcx] с [rdx, rdx + rcx]
; См. str_cmp
str_cmp_unsafe:
    xor r10, r10 ; сдвиг от начала
    .loop:
        mov r8, [rdi + r10]
        mov r9, [rsi + r10]
        cmp r8, r9
        jb .less
        ja .greater
        
        inc r10
        cmp r10, rcx ; дошли до конца?
        je .eq
        jmp .loop
        
    .less:
        mov rax, -1
        ret
        
    .greater:
        mov rax, 1
        ret
        
    .eq:
        xor rax, rax
        ret

global parse_uint
; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax число, в rdx его (числа) длину в символах
; rdx = 0, если число прочитать не удалось
parse_uint:
    ; mov rsi, rax ; rsi содержит адрес строки, которую парсим
    push rax
    call count_digits_at_the_beginning
    test rax, rax ; если 0 цифр,
    pop rax
    jz .fail ; выходим
        
    push rax
    mov rax, max_unsigned_x64_as_string ; избегаем т. н. магических констант
    push rsi
    call string_length ; запишем в rax длину длиннейшего (в символах) 64-битного числа
    pop rsi
    pop r8 ; в r8 теперь длина входного числа в символах
    cmp r8, rax ; сравним с длиной длиннейшего (в символах) 64-битного числа
    ja .escape ; если в записи числа больше rax символов, это число не поместится в uint
    
    je .cmp_with_max_unsigned_x64_number ; если в записи числа ровно rax символов,
    ; сравним это число с самым большим 64-битным числом
        
    .string_to_uint: ; если мы здесь, всё точно в порядке, можно конвертировать число
        xor rcx, rcx ; сколько прочитано
        mov r9, 1 ; текущая степень 10 (на что будем умножать)
        mov r10, 0 ; прочитанное число
        
        .convert_loop:
            mov rax, [rsi + rcx] ; код текущей цифры
            sub rax, '0' ; численное значение цифры
            mul r9
            add r10, rax
            
            inc rcx ; увеличим (символов прочитано) на 1
            cmp rcx, r8 ; сравним с длиной числа в символах
            je .mov_r8_to_rdx_and_r10_to_rax_and_ret ; если дочитали, уходим
            
            mov rax, r9 ; обновим степень 10
            mov rdx, 10 ; для этого умножим ещё на 10
            mul rdx ; умножили
            mov r9, rax ; обновили степень 10
            jmp .convert_loop
    
    .cmp_with_max_unsigned_x64_number:
        mov rcx, rax ; длина max_unsigned_x64 и входного числа
        mov rax, rsi ; начало входной строки
        mov rdx, max_unsigned_x64_as_string
        push rsi
        call str_cmp_unsafe
        pop rsi
        cmp rax, 0
        ja .fail ; наше число в строком представлении больше максимального 64-битного
        jmp .string_to_uint ; если меньше или равно — начинаем переводить в uint
        ; xor rcx, rcx
        ; .cmp_loop:
            ; mov rdx, [max_unsigned_x64_as_string + rcx]
            ; cmp [rax + rcx], rdx
            ; ja .fail ; если код цифры числа больше соответствующего кода цифры
            ; наибольшего 64-битного числа, наше число не поместится в rax
            ; inc rcx
            ; cmp rcx, r8 ; если дошли до конца,
            ; je .string_to_uint ; конвертируем строку в число
            ; jmp .cmp_loop
    
    .mov_r8_to_rdx_and_r10_to_rax_and_ret:
        mov rax, r10
        mov rdx, r8
        ret
    
    .fail:
        xor rdx, rdx
        
    .escape:
        ret

global parse_int
; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax число, в rdx его (числа) длину в символах (включая знак, если он был) 
; rdx = 0, если число прочитать не удалось
parse_int:
    xor r9b, r9b ; если число положительно, 0 — нет знака, 1 — '+'
    mov rsi, rax ; rsi содержит адрес строки, которую парсим
    mov r8, rax ; в r8 будем хранить указатель на первую цифру числа
    cmp [rsi], byte '+' ; оптимизация: кодов цифр меньше кода символа '+' нет
    jb .fail ; если код прочитанного символа меньше кода '+', это не цифра и не '+'
    je .plus_sign ; это '+'
    cmp [rsi], byte '-'
    jb .fail ; если код символа меньше кода '-' и символ — это не '+', то это не цифра
    jne .cmp_rax_with_max_signed_x64_as_string ; если не '-', это может быть цифра
    inc r8
    inc rax
    jmp .cmp_rax_with_min_signed_x64_as_string
    
    .fail:
        xor rdx, rdx
        
    .escape:
        ret

    .plus_sign:
        inc r9b
        inc r8
        inc rax
        jmp .cmp_rax_with_max_signed_x64_as_string
        
    .cmp_rax_with_min_signed_x64_as_string:
        push rsi
        push r8
        mov rdx, min_signed_x64_as_string
        call str_cmp
        pop r8
        pop rsi
        cmp rax, 1 ; если текущая строка больше min_signed,
        jne .fail ; выходим
        mov rax, r8 ; в r8 лежит (rsi+1), так как rsi[0] == '-'
        call parse_uint ; теперь в rax лежит число после '-'
        inc rdx ; длина числа увеличивается на 1
        neg rax ; x => -x
        ret
            
    .cmp_rax_with_max_signed_x64_as_string:
        push r8
        push r9w
        push rsi
        mov rdx, max_signed_x64_as_string
        call str_cmp
        pop rsi
        pop r9w
        pop r8
        cmp rax, 1 ; если число после '+' больше max_signed_x64,
        je .fail ; уходим
        push r9w
        mov rax, r8
        call parse_uint
        pop r9w
        xor r9b, r9b ; если знака нет,
        jz .escape ; ответ уже в rax и rdx (_parse_uint так возвращает значение)
        inc rdx ; (длина числа в символах) + 1
        ret
            

; Принимает source, destination и n в rsi, rdi, rax соответственно
; Копирует n символов из source в destination
string_copy_n_times_unsafe:
    .loop:
        test rax, rax ; если осталось скопировать 0 символов,
        jz .done ; всё готово
        dec rax ; небольшая оптимизация, чтобы не писать rax-1
        mov rdx, [rsi + rax]
        mov [rdi + rax], rdx
        jmp .loop
    
    .done:
        ret

global string_copy
; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    ; как и в других функциях этой библиотеки, первый параметр лежит в
    ; rax, второй — в rdx. Третий параметр я решил читать из rcx.
    ; Логичнее было бы использовать rsi, rdi, rax соответственно.
    push rax
    push rdx 
    push rcx
    call string_length
    mov r8, rax
    pop rcx
    pop rdx
    pop rax
    
    inc r8 ; добавим 1 к длине строки, чтобы ещё записать '\0'
    cmp r8, rcx ; если не помещается,
    ja .too_long ; завершаем
    dec r8 ; r8 = (длина строки без '\0')
    
    add rdx, rcx
    cmp rax, rdx ; если (адрес строки) >= (адрес буфера) + (длина буфера),
    sub rdx, rcx
    jae .it_is_safe_to_write_directly ; можно писать сразу
    
    add rax, r8
    cmp rax, rdx ; аналогично: если (адрес строки) + (длина строки) <=
    sub rax, r8
    jae .it_is_safe_to_write_directly ; <= (адрес буфера), всё хорошо
    
    ; inc r8 ; нуль-терминатор тоже пишем
    .write_to_the_stack_first:
        mov r10, r8
        shr r10, 1 ; делим на 2
        
        .push_words_for_temp_buffer:
            test r10, r10
            jz .maybe_one_byte_will_be_needed
            push word 0
            dec r10
            jmp .push_words_for_temp_buffer
        
        .maybe_one_byte_will_be_needed:
            mov r9b, r8b
            and r9b, 1 ; остаток от деления на 2
            test r9b, r9b
            jz .enough_words
            push word 0
        
        .enough_words:
            mov rsi, rax
            mov rdi, rsp ; куда будем писать (где начинаются наши word-ы)
            mov rax, r8
            
        .copy_from_source_to_the_stack:
            push r8
            push rax
            push rdx
            call string_copy_n_times_unsafe
            pop rdx
            pop rax
            pop r8
            
        .copy_from_the_stack_to_destination_then_write_null:
            mov rsi, rsp
            mov rdi, rdx ; destination — это указатель на буфер
            mov rax, r8
            push r8
            push rdx
            call string_copy_n_times_unsafe
            pop rdx
            pop r8
            mov byte [rdx + r8], 0 ; запишем '\0' в буфер
            mov rax, r8
            ret
    
    .it_is_safe_to_write_directly:
        inc r8 ; нуль-терминатор тоже пишем
        push r8 ; длина строки с '\0'
        mov rsi, rax
        mov rdi, rdx
        mov rax, r8
        call string_copy_n_times_unsafe
        
        ; .pop_rax_from_the_stack_and_decrement_it_by_one_then_return:
        pop rax ; rax = (длина строки + 1)
        dec rax
        ret
        
    .too_long:
        mov rax, 0
        ret    

; test_message: db "Hello, world!", 0    
; global _start    
; _start:        
    ; mov rdi, 0
    ; call _exit
