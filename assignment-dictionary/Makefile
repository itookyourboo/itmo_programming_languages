NASM=nasm -felf64 -o
LD=ld -o

lib.o: lib.asm
	$(NASM) lib.o lib.asm
dict.o: dict.asm lib.o
	$(NASM) dict.o dict.asm
main.o: main.asm dict.o words.inc
	$(NASM) main.o main.asm
program: main.o dict.o lib.o
	$(LD) program dict.o main.o lib.o

.PHONY: clean
clean:
	rm -f ./*.o

