compiler_cpp = g++
compiler_asm = cc

main: main.o assembly.o
	$(compiler_cpp) -o bfcomp main.o assembly.o
	./bfcomp -f bfcodes/hello-world.b -s code -b 100

rm:
	rm *.o bfcomp a.out

asm:
	$(compiler_asm) out.s
	./a.out
