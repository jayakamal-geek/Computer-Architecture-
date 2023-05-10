;**************************************************************************************************
;   Name        : Maddula Jaya Kamal
;   Roll Number : CS19B028
;   Application : This code takes a string input and Encrypts the digits in it
;   Question    : CS2610 Assignment-2
;   Input       : string
;   output      : string
;**************************************************************************************************

%include "io.mac"   ;Header File kind of inclusion to get pre written functions


section .data       ;//initialised Data
    intro_msg       db      'Enter your string here : ', 0  ;//Prompting for string input
    query_msg       db      'Do YOU WANT TO RUN THE PROGRAM ON ANOTHER INPUT? (Y/N) :', 0      ;//Initialising the query message


section .bss      ;//uninitialised Data
    string_input    resb     20     ;//defining buffer size for the string
    query_response  resb     1      ;//defining buffer size for input as answer for query response

section .text
    global _start       ;defining label to the entrypoint of the code

_start:
    PutStr      intro_msg               ;//prompting user to enter string input
    GetStr      string_input, 20        ;//Getting input for the string
    mov         EBX, string_input       ;//moving input adress into a register

    loop:       ;//loop to visit each element of the string                                       
        mov         AL, [EBX]       ;//getting absolute value at the address of EBX
        inc         EBX             ;//incrimenting pointer value to reach next element
        cmp         AL, 0           ;//checking if we reached the end of the string
        je          term_prompt     ;//if end of loop is reached then jump to the end of the loop
        cmp         AL, '0'         ;//checking its a digit or not
        jl          print_label     ;//proceed to printing
        cmp         AL, '9'         ;//checking if its a digit or not
        jg          print_label     ;//proceed to printing
        cmp         AL, '5'         ;//seeing if digit is greater than 5 or less than 5
        jl          add_step        ;//jumping to relavent label
        sub         AL, 5           ;//encrypting digits by subtracting 5
        jmp         print_label     ;//print digit after making correction

        add_step:
            add     AL, 5           ;//encrypting digits by adding 5

        print_label:
            PutCh   AL              ;//printing the string unit one by one
            jmp     loop            ;//jump back to loop label to perform another iteration




    term_prompt:    ;//prompt for terminationg loop or iterating again
        nwln    ;//asking to print a new Line
        nwln    ;//asking to print a new Line
        PutStr      query_msg               ;//asks user if he wants to terminate program or continue
        GetCh       [query_response]          ;//reads user response
        cmp         byte [query_response], 'y'     ;//compare response
        je          _start                  ;//jump back to start
        cmp         byte [query_response], 'Y'     ;//compare response
        je          _start                  ;//jump back to start

    .EXIT   ;//exit the program when this state is reached