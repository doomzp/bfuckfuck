compiler = g++

main: main.o parse.o
	$(compiler) -o bfcomp main.o parse.o
	./bfcomp bfcodes/incdec.bf

rm:
	rm *.o bfcomp
