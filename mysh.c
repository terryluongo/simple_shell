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

		buf[strlen(buf)-1] = '\0';

		char *buf_copy, *cur_pipe, *saveptr1, *saveptr2;
		buf_copy = buf;
		int i;

		for(i = 0; ; i++, buf_copy = NULL) {
			cur_pipe = strtok_r(buf_copy, "|", &saveptr1);

			if (cur_pipe == NULL) {
				break;
			}
			// idea: pipe each time
			// run has two args, read and write 
			// on first run, has to be rd from stdin
			// on NULL run, has to be write to stdout
			// each non-null cur_pipe creates new pipe 
			// save prev outside scope 
			// then submit the read of the prev and the write of new to run function

			char *tok[BUF_MAX];

			for (int j = 0; ; j++, cur_pipe = NULL) {
				tok[j] = strtok_r(cur_pipe, " ", &saveptr2);
			
				if(tok[j] == NULL) {
					run(tok);
					break;
				}
			}
		}	

		for (int waits = 0; waits < i; waits++) {
			wait(0);
		}

		write(0,"$",1);	
	}
}

void run(char *program[]) {
	pid_t child;
	child = fork();

	if (child == 0) {
		execvp(program[0],program);
		exit(42);
	}	
}





