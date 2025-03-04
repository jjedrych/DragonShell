dragonshell: dragonshell.o dragoncmds.o
	gcc -g -Wall dragonshell.o dragoncmds.o -o dragonshell

compile: dragonshell.c dragoncmds.c
	gcc -c dragonshell.c dragoncmds.c

debug: dragonshell.c dragoncmds.c
	gcc dragoncmds.c dragonshell.c -g -o dragonshell
	gdb dragonshell

clean:
	rm *.o dragonshell

run: dragonshell
	./dragonshell

