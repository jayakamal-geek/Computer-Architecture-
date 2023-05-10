%include "io.mac"   ;Header File kind of inclusion to get pre written functions


section .data       ;//initialised Data
    intro_msg       db      'Enter your string here : ', 0  ;//Prompting for string input
    query_msg       db      'Enter Input String Size: ', 0  ;//prompting for input


section .bss      ;//uninitialised Data
    K    resb     20     ;//defining buffer size for the string
    L    resd     1      ;//1 double for int size
    N    resd     1      ;//4 bytes for result

section .text
    global _start       ;defining label to the entrypoint of the code

_start:
    PutStr  intro_msg   ;//prompting user to enter string input
    GetStr  K   ;//Getting input for the string
    PutStr  query_msg   ;//prompting for string size
    GetLInt [L] ;//Input for string Size

    mov EAX, 0  ;Initialising EDX with 0
    mov ECX, 0  ;Initialising ECX with 0
    mov EBX, 10 ;Initialising EBX with 10

    LOOP:
        movzx EDX, byte[K+ECX]  ;//getting the required character
        cmp EDX, 0 ;//comparing the values with 0
        je END
        sub EDX, 48 ;//subtracting 48 from EDX
        inc ECX ;//incrimenting ECX
        push EDX    ;//pushing EDX to not lose value after MUL
        mul EBX ;//multiplying with 10
        pop EDX ;
        add EAX, EDX    ;//adding EDX to the 10 times of previous result
        jmp LOOP    ;//jumping for next iteration

    END:
        mov [N], EAX    ;//moving result into allocated space
        PutLInt EAX ;//printing result
        nwln
.EXIT
