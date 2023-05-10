;**************************************************************************************************
;   Name        : Maddula Jaya Kamal
;   Roll Number : CS19B028
;   Application : This is a code to find nth number of fibanocci series using recursion
;   Question    : CS2610 Assignment-3
;   Input       : integer
;   output      : integer
;**************************************************************************************************

%include "io.mac"   ;Header File kind of inclusion to get pre written functions

section .data       ;//initialised Data
section .bss      ;//uninitialised Data

section .text
    global _start       ;//defining label to the entrypoint of the code

_start:
    GetLInt ECX      ;//Index of fibanocc number we need //Store the value of n
    push ECX         ;//pushing ECX to stack
    rdtscp           ;//Read-Time-stamp counter and Processor ID
    pop ECX          ;//popping out value of n 
    mov EBX, EAX     ;//moving EAX value into EBX
    mov EDX, 0       ;//clearing out garbage from EDX
    mov EAX, 0       ;//0th term of the fibanocci series //Fibanocci(0)
    cmp ECX, 0       ;//if 0th element is asked //if 0th element is asked jump to end print 0
    je Print         ;//jump to label zero  //jump to label zer0
    call Fibanocci   ;//making the first function call

Print:
    PutLInt EAX      ;//printing the output
    nwln             ;//printing a new Line

    rdtscp           ;//Read-Time-stamp counter and Processor ID
    sub EAX, EBX     ;//getting difference of cycles          
    PutLInt EAX      ;//printing the lower order 32 bits of clock cycle count
    nwln             ;//printing newline after the output
.EXIT                ;//exiting the program


;**************************************************************************************************
;   Name of the Function : Fibanocci
;   Role of the Function : Computes the nth term of fibanocci sequence
;   Type of Function     : Recursive
;**************************************************************************************************

Fibanocci:           ;//label of the function
    cmp ECX, 2       ;//compare value of ECX(n) with 2
    jg  recCase      ;//Jump to the recursive case
    mov EAX, 1       ;//base case of 1st and 2nd elements being 1
    ret              ;//returning the base case result

recCase:
    push EDX         ;//push the current value of EDX into stack
    dec  ECX         ;//decriment ecx to find Fibanocci(n-1)
    call Fibanocci   ;//calling fibanocci function to compute value of (n-1)th term
    mov EDX, EAX     ;//moving the value of Fibanocci(n-1) into EDX
    dec ECX          ;//decriment ecx to find Fibanocci(n-2)
    call Fibanocci   ;//calling fibanocci function to compute value of (n-2)th term
    add EAX, EDX     ;//adding Fibanocci(n-1) and Fibanocci(n-1) and store in EAX
    add ECX, 2       ;//adding 2 to restore initial n 
    pop EDX          ;//popping out EDX
    ret
