;**************************************************************************************************
;   Name        : Maddula Jaya Kamal
;   Roll Number : CS19B028
;   Application : This is a code to implement binary search
;   Question    : CS2610 Assignment-4
;   Input       : integer array(search space) and an integer(search subject)
;   output      : integer(1(found), 0(not found))
;**************************************************************************************************


%include "io.mac"   ;//Including prewritten functions and macros

section .data   ;//initialised data
    maxSize         EQU     100     ;//defining the maximum size of an array a constant    

section .bss    ;//uninitialised data
    inputArr        resd    maxSize ;//defining space for the array
    query_response  resb     1      ;//defining buffer size for input as answer for query response

section .text   ;//the code goes under this section
    global _start   ;//defining the start label, entry point to the code

_start:
    GetLInt EBX   ;//Taking the input for number of inputs
    mov ESI, 0      ;//Initialising ESI with 0
	
InputLoop:
    cmp ESI, EBX     ;//compare loop index with array size
    je  SearchIn    ;//jumping to search after all inputs are taken
    GetLInt EAX     ;//taking input for the array
    mov [inputArr + 4*ESI], EAX  ;//Entering taken input into the array
    inc ESI          ;//incrimenting the array Index
    jmp InputLoop    ;//controlling loop flow

SearchIn:
   
    GetLInt EDX     ;//taking input for search query
    push EDX        ;//pushing search query into stack for stack mode argument passing
    push EBX         ;//pushing array size into stack for stack mode argument passing
    push inputArr   ;//pushing array address into stack for stack mode argument passing
    call BinarySearch   ;//making the function call
    nwln            ;//printing a new line
    pop EAX         ;//popping out the array length
    pop EBX
    pop EDX         ;//popping out Search Query
    jmp term_prompt     ;//Exit Code if query is not found

term_prompt:    ;//prompt for terminationg loop or iterating again
    GetCh       [query_response]                ;//reads user response
    cmp         byte [query_response], 'y'      ;//compare response
    je          ExitCode                        ;//jump back to start
    cmp         byte [query_response], 'Y'      ;//compare response
    je          ExitCode                        ;//jump back to start
    jmp          SearchIn                      ;//jumps to SearchIn to take new input for the code

ExitCode:       ;//label to exit the Code
    .EXIT       ;//Exit the code when this point is reached

BinarySearch:   ;//label for BinarySearch function
    enter 0, 0  ;//setting the new stack frame
    push EAX    ;//Pushing the registers that will be used in the function
    push EDX     ;//Pushing the registers that will be used in the function
    push EBX    ;//Pushing the registers that will be used in the function
    mov EAX, [EBP+8]    ;//copying array address from stack 
    mov EDX, [EBP+12]    ;//copying size of array from stack 
    mov EBX, [EBP+16]   ;//copying search query from stack
    mov ECX, 0           ;//low
    dec EDX              ;//Decrimenting EDX to get last index

RecLoop:        ;//the loop for iteration
    cmp ECX, EDX  ;//iterate loop only when low < high
    jg ElemNotFound ;//when high exceeds low search is finished
    mov ESI, ECX      ;//loading ESI with value of low
    add ESI, EDX      ;//Add value in ESI and EDX and place it in ESI
    shr ESI, 1       ;//performing a right shift on the integer by one digit
                    ;//equivalnt to dividing the number by 2 but faster.
    cmp EBX, [EAX+ESI*4] ;//comparing search query with digit at the middle index
    je  ElemFound   ;//if the element equals then it means we found the query
    jl  FirstHalfSearch ;//if the element is less than middle element then search first FirstHalfSearch

SecondHalfSearch:   ;//label for part of code to search second half
    inc ESI          ;//incrimenting to make it new low
    mov ECX, ESI      ;//updating the new low value
    jmp RecLoop     ;//repeat the process again for upper half of the subarray
    
FirstHalfSearch:    ;//label for part of code to search first half
    dec ESI          ;//decrimenting to update the new high
    mov EDX, ESI      ;//updating the new high
    jmp RecLoop     ;//repeat the process on lower half of the code

ElemFound:
    PutLInt 1       ;//outputting 1 if the element is found in the array
    jmp Return      ;//jump to return

ElemNotFound:
    PutLInt 0       ;//outputting 0 if the element is not found in the array

Return:
    pop EBX         ;//popping out the registers while leaving the function
    pop EDX          ;//popping out the registers while leaving the function
    pop EAX         ;//popping out the registers while leaving the function

    leave           ;//removing EBP //deleting the functions stack frame
ret


    


