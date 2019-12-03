CC = gcc -Wall -c

todo : shell twc
shell: shell.o 
	gcc  obj/shell.o -o bin/shell -lpthread -lm
shell.o: src/shell.c
	$(CC)  src/shell.c -o obj/shell.o -lpthread -lm

twc: twc.o 
	gcc  obj/twc.o -o bin/twc -lpthread -lm
twc.o: src/twc.c
	$(CC)  src/twc.c -o obj/twc.o -lpthread -lm

.PHONY: clean
	clean: rm bin/* obj/*
