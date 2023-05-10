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
    fileName    db  'input.txt', 0  ;//defining the file name as a constant
    errorMsg    db  'Error processing the interrupt', 0 ;//error message while processing input

section .bss    ;//uninitialised data
    inputNum    resd    1   ;//storing a 4byte data for the integer
    str_len     resd    1   ;//variable to store stringLength
    inputStr    resb    11  ;//string to store input number
    revStr      resb    11  ;//array to store reverse    
    mmapAddr    resd    1   ;//storing a 4byte mmap address
    fd_create   resw    1   ;//storing file descriptor obtained after the create function

section .text   ;//the code goes under this section
    global _start   ;//defining the start label, entry point to the code

_start: ;//telling linker that the code starts here
    GetStr inputStr ;//taking the input for the integer as a string
    call stringLength   ;//code to find the stringLength of the string

    ;//creating a file with name input.txt
    mov EAX, 8  ;//moving the required value to EAX to create file
    mov EBX, fileName   ;//moving file name into EBX to create file with that name
    mov ECX, 0777   ;//file permissions
    int 0x80    ;//calling the interrupt to create the file with required name
    cmp EAX, 0  ;//check for errors
    jl Error

    mov [fd_create], EAX    ;//getting file descriptor into assigned byte of info

    ;//writing to the file
    mov EAX, 4   ;//moving the required value to EAX to write to a file
    mov EBX, [fd_create]    ;//moving the file descriptor to EBX
    mov ECX, inputStr   ;//moving the value to be written to ECX
    mov EDX, [str_len]  ;//moving number of bytes of info need to be written to EDX
    int 0x80    ;//calling interrupt to write to a file
    cmp EAX, 0  ;//check for errors
    jl Error

    ;//closing the file
    mov EAX, 6  ;//moving the required value to EAX to close a file
    mov EBX, [fd_create]    ;//moving the file descriptor to EBX
    int 0x80    ;//calling the interrupt to close a file
    cmp EAX, 0  ;//check for errors
    jl Error

    ;//opening an existing file
    mov EAX, 5  ;//system call for opening file
    mov EBX, fileName   ;//loadinf file name into EBX
    mov ECX, 0  ;//read mode
    mov EDX, 0  ;//
    int 0x80    ;//calling an interrupt
    cmp EAX, 0  ;//check for errors
    jl Error

    mov [fd_create], EAX    ;//moving file descriptor from EAX to fd variable

    ;//mmaping the file using mmap2
    mov EAX, 0xc0   ;//System call of mmap2
    XOR EBX, EBX    ;//initialising EBX with 0
    mov ECX, 4096   ;//initialise ECX by asking one page of data
    mov EDX, 1  ;//initialise EDX with 1
    mov ESI, 2  ;//shared mapping mode
    mov EDI, [fd_create]    ;//moving file descriptor
    XOR EBP, EBP    ;//initialise EBP with 0
    int 0x80    ;//calling the interrupt
    cmp EAX, -1
    je Error

    mov [mmapAddr], EAX ;//moving mmap from register into allocated variable
    
    ;//closing the file
    mov EAX, 6  ;//moving the required value to EAX to close a file
    mov EBX, [fd_create]    ;//moving the file descriptor to EBX
    int 0x80    ;//calling the interrupt to close a file
    cmp EAX, 0  ;//check for errors
    jl Error

    call StringToInt    ;//code to convert string into Int

    ;//calling a fork
    mov EAX, 0x2    ;//loading the system call value of fork into EAX
    int 0x80    ;//calling an interrupt

    cmp eax, 0  ;//status is child if EAX is 0
    jz processChild ;jump to child process

    ;//parent process
    processParent:
            call reverseNum ;//reversing and printing a given input number

    ;//waiting for child process to complete
    mov EAX, 0x7    ;//system call to ask processor to wait
    mov EBX, -1
    XOR ECX, ECX    ;//initialise ECX to 0
    XOR EDX, EDX    ;//initialise EDX to 0
    int 0x80    ;//calling the interrupt

    ;//print parent process result
    PutStr revStr   ;//printing result of parent process
    nwln
    jmp exit    ;//jump to end of the code

    processChild:
        call checkPrime ;//checking whether the input number is prime number or not

    jmp exit

    Error:
        PutStr errorMsg ;//printing error message
        nwln    ;//printing a new line

exit:
    .EXIT   ;//exiting the code analogous to "return 0" in main function


checkPrime:     ;//function to check whether a given number is prime or not
    mov ECX, 1  ;//loop variable or the factor
    XOR EBX, EBX    ;//initialising EBX to 0

    divLoop:
        mov EAX, [inputNum] ;//moving inputNum into EAX to enable division
        XOR EDX, EDX    ;//initialising EDX to 0
        inc ECX         ;//incrimenting ECX to 0
        cmp ECX, EAX    ;//compare ECX with EAX
        jge printRes    ;//if number is reached then print the result
        div ECX         ;//dividing EAX with ECX
        cmp EDX, 0      ;//compare remainder with 0
        je flipValue    ;//the number is not a prime Number
        jmp divLoop     ;//reiterate the loop

    flipValue:
        mov EBX, 1  ;//changes value to 1

    printRes:
        PutLInt EBX     ;//print whether input is prime or not
        nwln    ;//printing new line

ret ;//exiting the function and returning the control to start function


reverseNum:     ;//function to reverse a given integer
    mov EAX, inputStr ;//moving inputNum into EAX to enable division
    mov EDX, revStr   ;//array to store the reverse string
    mov EBX, EAX    ;//getting adress into EBX
    add EAX, [str_len]  ;//adding offset to come from reverse

    revLoop:
        cmp EAX, EBX    ;//compare EAX with base address
        je end  ;//jump to the end after reversing is done
        dec EAX ;//decrimenting the address in EAX
        mov cl, [EAX]    ;//moving the value in content of EAX into EDX
        mov [EDX], cl    ;//moving the value in content of EAX into EDX
        inc EDX ;//incrimenting EDX
        jmp revLoop ;//reiterate

    end:
        mov byte [EDX], 0    ;//putting null char at the end

ret ;//exiting the function and returning the control to start function


;//function to convert string into integer
StringToInt:
    XOR EAX, EAX    ;//initialising EAX to 0
    mov EBX, [mmapAddr] ;//copying string address into EB
    mov ECX, 10     ;//initialise ECX to 10

    ConvertLoop:
        cmp byte [EBX], 48   ;//reaching spaces
        jl done ;//jump when other than ints
        cmp byte [EBX], 57   ;//other chars than ints
        jg done ;//jump when other than ints
        XOR EDX, EDX    ;//initialising edx to 0
        mul ECX ;//multiply ECX with EAX
        movzx EDX, byte[EBX]  ;//copy char into EDX
        sub EDX, 48     ;//subtract the offset of 0
        add EAX, EDX   ;//add EAX to EDX and store in EDX
        inc EBX
        jmp ConvertLoop ;//jump to iteration

    done:
        mov [inputNum], EAX ;//move eax value into inputNum

ret


;//function to find the length of the string
stringLength:
    mov EAX, inputStr   ;//moving input string variable into EAX
    XOR ECX, ECX    ;//initialising ECX to 0

    lengthLoop:
        cmp byte [EAX], 0   ;//comparing EBX content value to 0
        je Ldone;//jump to final step
        inc EAX ;//incrimenting EBX
        inc ECX ;//incrimenting count
        jmp lengthLoop  ;//iterating

    Ldone:
        mov [str_len], ECX  ;//moving into assigned variable

ret