;**************************************************************************************************
;   Name        : Maddula Jaya Kamal
;   Roll Number : CS19B028
;   Application : This is a code to find nth number of fibanocci series using iteration
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
    rdtsc            ;//Read-Time-stamp counter
    push EAX         ;//pushing EAX value to stack
    mov EAX, 0       ;//0th term of the fibanocci series //Fibanocci(0)
    mov EDX, 1       ;//1st term of the fibanocci series //Fibanocci(1)
    mov EBX, 0       ;//temporary variable to store values of Fibanocci(i-1) while calculating Fibanocci(i)
    cmp ECX, 0       ;//if 0th element is asked //if 0th element is asked jump to end print 0
    je Zero          ;//jump to label zero  //jump to label zer0
    dec ECX          ;//decrimenting loop count by 1

Fibanocci:           ;//label of loop to calculate fibanocci
    cmp ECX, 0       ;//compare loop variable with zero
    je  Print        ;//jump on reaching the end of the loop
    mov EBX, EDX     ;//moving last element into temporary element
    add EDX, EAX     ;//adding to get result element
    mov EAX, EBX     ;//updating penultimate element
    dec ECX          ;//decrimenting the loop variable
    jmp Fibanocci    ;//a loop statement that has loop parameter as ECX

Print:
    PutLInt EDX      ;//printing the output
    jmp END          ;//jump to end after printing the result

Zero:
    PutLInt EAX      ;//printing the output

END:
    nwln             ;//printing a new Line
    pop EBX          ;//popping start cycle count
    rdtsc            ;//Read-Time-stamp counter
    sub EAX, EBX     ;//compute difference to find time taken
    PutLInt EAX      ;//printing the lower order 32 bits of clock cycle count
    nwln             ;//printing new lines

.EXIT                ;//exiting the program