all: assembler.c
	gcc -g assembler.c -o assembler
	./assembler ./samples/pong.asm
	diff ./a.obj ./pong.obj > diff


clean: 
	rm assembler a.obj
