compiler_cpp = g++
compiler_asm = gcc

main: main.o parse.o assembly.o
	$(compiler_cpp) -o bfcomp main.o parse.o assembly.o 
	./bfcomp bfcodes/incdec.bf

rm:
	rm *.o bfcomp out.s a.out

asm:
	$(compiler_asm) out.s
	./a.out