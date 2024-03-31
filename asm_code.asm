%macro change 2
mov edi, %2
mov rax, %1
mov ebx, 10
xor ecx, ecx
call repeat
mov byte [edi], 0  
%endmacro
%macro imprimeEnPantalla 2
   mov     eax, 4
   mov     ebx, 1
   mov     ecx, %1
   mov     edx, %2
   int     0x80
%endmacro
 %macro leeTeclado 0
  mov     eax,     3
   mov     ebx,     0
    mov     ecx,     entrada
     mov     edx,     100
      int     0x80
   %endmacro
%macro suma 2
   mov rax, %1
   add rax, %2
%endmacro
%macro resta 2
  mov rax, %1
 sub rax, %2
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
entrada resb 100
section .data
msgError db "Not a number", 0
msgErrorlen equ $ - msgError
msg db "-", 0
msglen equ $ - msg
newline db 10
lngnewline equ $-newline
section .text
global _start
process_entrada:
mov rbx, 0
mov rdx,0
mov rax,0
mov rcx,0
mov r9,0
mov r8,0
mov al,[esi]
cmp al,45
je entrada_negativa
mov rax, 66
push rax
jmp iterate_entrada
entrada_negativa:
mov rax, 67
push rax
inc esi
 iterate_entrada:
mov rax,0
mov al, [esi]
 test al,al
jz endloop
cmp al,10
je endloop
cmp al,48
jl imprimir_error_entrada
cmp al, 57
jg imprimir_error_entrada
sub rax, 48
push rax ; el numero actual esta en la pila
mov rax, rcx ;rcx es el atentior
mov rbx,10 ; multiplico el numero anterior en rax por 10, queda en rdx
mul rbx
mov rcx, rax
pop rax 
add rcx, rax
inc esi
jmp iterate_entrada
endloop:
pop rax
cmp rax,67
jne no_negativo
mov rax, 0
sub rax, rcx
mov rcx, rax
no_negativo:
ret
imprimir_error_entrada:
imprimeEnPantalla msgError,msgErrorlen
jumpline
jmp exit
print_number:
cmp rax, 0
jl negative
change rax, numero
call print
ret
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
mov rax, rbx
change rbx, numero
call print
ret
print:
mov eax, 4
mov ebx, 1
mov ecx, numero
mov edx, 22
int 0x80
jumpline
ret
repeat:
xor edx, edx
div ebx
add dl, '0'
push dx
inc ecx
test rax, rax
jnz repeat
jmp pop_chars
pop_chars:
pop dx
mov [edi], dl
inc edi
loop pop_chars
ret
_start:
     mov rax, 1 
 push rax
     mov rax, 5 
 push rax
mov rbx, qword [rsp + 0] 
 mov rcx, 2 
 resta rbx, rcx 
 push rax 
mov rbx, qword [rsp + 16] 
 mov rcx, qword [rsp + 0] 
 suma rbx,rcx 
 push rax 
     mov rax, qword [rsp + 0] 
 push rax
mov rbx, qword [rsp + 32] 
 mov rcx, 5 
 resta rbx, rcx 
 push rax 
     mov rax, qword [rsp + 0] 
 push rax
leeTeclado
mov esi,entrada
call process_entrada 
 push rcx
mov edi, numero
mov ecx, 22
xor al, al
rep stosb
mov rax, [rsp + 24]
call print_number
mov rbx, qword [rsp + 0] 
 mov rcx, 4 
 suma rbx,rcx 
 push rax 
mov edi, numero
mov ecx, 22
xor al, al
rep stosb
mov rax, [rsp + 0]
call print_number
exit:
 mov eax,1
mov ebx, 0
int 80h
