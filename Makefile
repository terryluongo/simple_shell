mysh: mysh.c
	gcc -Wall -pedantic -o mysh mysh.c


PHONY: clean
clean:
	rm mysh

