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
     mov qword [rsp - 0], 1 
     mov qword [rsp - 0], 5 
mov rbx, qword [rsp - 8] 
 mov rcx, 2 
 resta rbx, rcx 
 push rax 
mov rbx, qword [rsp - 24] 
 mov rcx, qword [rsp - 0] 
 suma rbx,rcx 
 push rax 
    mov rax, qword [rsp - 0] 
      mov qword [rsp - 16], rax 
mov rbx, qword [rsp - 40] 
 mov rcx, 5 
 resta rbx, rcx 
 push rax 
    mov rax, qword [rsp - 0] 
      mov qword [rsp - 8], rax 
mov eax,1
mov ebx, 0
int 80h
