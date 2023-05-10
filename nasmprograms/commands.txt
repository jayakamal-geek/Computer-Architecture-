nasm -f elf demo.asm
ld -m elf_i386 -s demo.o io.o -o demo
./demo
