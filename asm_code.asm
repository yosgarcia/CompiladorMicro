%macro imprimeEnPantalla 2
   mov     eax, 4         
   mov     ebx, 1            
   mov     ecx, %1               
   mov     edx, %2                
   int     0x80
%endmacro
%macro suma 2
   mov rax, %1      ; Mueve el primer argumento a eax
   add rax, %2      ; Suma el segundo argumento a eax
%endmacro
%macro resta 2
  mov rax, %1      ; Mueve el primer argumento a eax
 sub rax, %2      ; Resta el segundo argumento a eax
%endmacro
section .data
   msg db "Input value: ", 0
   msglen equ $ - msg
section .text
   global _start
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
mov eax,1
mov ebx, 0
int 80h
