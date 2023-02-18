compiler_cpp = g++
compiler_asm = cc

main: main.o parse.o assembly.o
	$(compiler_cpp) -o bfcomp main.o parse.o assembly.o 
	./bfcomp bfcodes/x.bf

rm:
	rm *.o bfcomp a.out

asm:
	$(compiler_asm) out.s
	./a.out
