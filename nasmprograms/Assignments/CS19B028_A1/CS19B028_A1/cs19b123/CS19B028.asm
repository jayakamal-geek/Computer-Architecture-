;This is a program to compute the sum of N numbers, N is input given by user.

%include "io.mac"

section .data
section .bss


section .text
    global _start       ;defining label to the entrypoint of the code

_start:
    GetLInt ECX              ;takes in the input
    mov EDX, 0               ;initialising register to 0

loop:                        ;label for the start of the loop
    cmp ECX, 0               ;comparing ecx with zero to end the loop when zero is reached
    jle END                  ;signal to exit loop
    GetLInt EAX              ;taking input for number to be added
    add EDX, EAX             ;EDX = EDX + EAX
    dec ECX                  ;decrimenting ECX
    jmp loop                 ;return to the start of the loop

END:

    PutLInt EDX              ;printing the output

    .EXIT                    ;Line to Exit the program


