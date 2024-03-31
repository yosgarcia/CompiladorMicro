%macro change 2               ; Save the pointer to the buffer

    mov edi, %2               ; Destination pointer (buffer)
    mov rax, %1               ; Load the number to convert
    mov ebx, 10               ; Set base 10 for conversion
    xor ecx, ecx              

    jmp repeat  
    jmp pop_chars  
    mov byte [edi], 0  

%endmacro

%macro jumpline 0
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, lngnewline
    int 0x80
%endmacro


section .bss
    numero resb 22
section .data
    msg db "-", 0
   msglen equ $ - msg
   newline db 10
   lngnewline equ $-newline
section .text 
    global _start

_start:
    mov eax, 55
    push rax
    mov rax, 2147483690
    cmp rax, 0
    jl negative
    change rax, numero
    jmp imprimir
    


negative:
    push rax
    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, msglen
    int 0x80
    pop rax
    mov ebx, 0
    sub rbx, rax
    change rbx, numero
    
imprimir:
    mov eax, 4
    mov ebx, 1
    mov ecx, numero
    mov edx, 22
    int 0x80
    jumpline
    jmp exit


repeat:
    xor edx, edx              ; Clear upper 32 bits of edx
    div ebx                   ; Divide eax by 10, remainder in edx
    add dl, '0'               ; Convert remainder to ASCII character
    push dx                   ; Save the character on the stack
    inc ecx                   ; Increment the counter
    test rax, rax             ; Check if quotient is zero
    jnz repeat               ; If not zero, repeat the loop
    

    ; Pop characters from the stack and store in the buffer
pop_chars:
    pop dx                    ; Pop a character from the stack
    mov [edi], dl             ; Store the character in the buffer
    inc edi                   ; Move to the next position in the buffer
    loop pop_chars  
    mov byte [edi], 0         ; Null terminate the string
    jmp imprimir
    

exit:
    mov eax,1
    mov ebx, 0
    int 80h
