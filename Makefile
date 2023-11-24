mysh: mysh.c
	gcc -g -Wall -pedantic -o mysh mysh.c


PHONY: clean
clean:
	rm mysh

