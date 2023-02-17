compiler = g++

main: main.o parse.o assembly.o
	$(compiler) -o bfcomp main.o parse.o assembly.o 
	./bfcomp bfcodes/incdec.bf

rm:
	rm *.o bfcomp
