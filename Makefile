all: func_reg.o func_csv.o fornecidas.o main.o
	gcc func_reg.o func_csv.o fornecidas.o main.o -o programTrab
	rm *.o


all_windows: func_reg.o func_csv.o fornecidas.o main.o
	gcc func_reg.o func_csv.o fornecidas.o main.o -o programTrab
	del *.o

func_reg.o:
	gcc -c func_reg.c

func_csv.o:
	gcc -c func_csv.c

fornecidas.o:
	gcc -c fornecidas.c

main.o:
	gcc -c main.c

clean_windows:
	del /Q *.o programTrab

clean:
	rm -f *.o programTrab

run:
	./programTrab

runTest:
	rm estacoes_removidos.bin
	cp estacoes_removidos_original.bin estacoes_removidos.bin
	make all
	./programTrab < in