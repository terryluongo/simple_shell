#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


#define BUF_MAX 4096

void run(char *program[]);
int main(int argc, char *argv[]) {
	char buf[BUF_MAX];
	write(0, "$", 1);
	while(fgets(buf, BUF_MAX, stdin) != NULL) {
		if (buf[0] == '\n') {
			continue;
		}
		buf[strlen(buf)-1] = '\0';
		char *tok[BUF_MAX];
		tok[0] = strtok(buf, " ");
		int i = 1;
		// i feel like we want to tokenize first wrt | then wrt " "
		while((tok[i] = strtok(NULL, " ")) != NULL) {
			i++;
		}
		tok[i] == NULL;
		run(tok);
			
		
		write(0,"$",1);
		
	}
}
void run(char *program[]) {
	pid_t child;
	int exit_value;
	child = fork();
	if (child == 0) {
		execvp(program[0],program);
		exit(42);
	}
	else {
		wait(&exit_value);
	
	}
}





