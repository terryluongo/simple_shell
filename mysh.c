#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


#define BUF_MAX 4096

void run(char *program[], int in, int out);

int main(int argc, char *argv[]) {

	char buf[BUF_MAX];

	write(0, "$", 1);

	while(fgets(buf, BUF_MAX, stdin) != NULL) {

		buf[strlen(buf)-1] = '\0';

		char *buf_copy, *cur_exp, *saveptr1, *saveptr2;
		buf_copy = buf;
		int i, prev_read;
		prev_read = 0;
		int fds_open[1024];
		int fds_open_index = 0;

		char *tok[BUF_MAX];
		for(i = 0; ; i++, buf_copy = NULL) {
			
			int cur_pipe[2];
			
			cur_exp = strtok_r(buf_copy, "|", &saveptr1);
			
			// not the first call, we can call pipe if it is not null
			if (buf_copy == NULL) {

				// if NULL, no new pipe needed, just run previous exp.
				if (cur_exp == NULL) {
					run(tok,prev_read,1);
				}

				// not the first and not NULL, need pipe
				else {
					if (pipe(cur_pipe) != 0) {
						printf("replace with perror");
						exit(0);
					}
					
					fds_open[fds_open_index] = cur_pipe[0];
					fds_open[fds_open_index+1] = cur_pipe[1];
					fds_open_index += 2;

					run(tok,prev_read,cur_pipe[1]);

					prev_read = cur_pipe[0];	
				}
			}

			else {
				prev_read = 0; //INPUT REDIRECTION GO HERE
			}
			
			if (cur_exp == NULL) break;

			for (int j = 0; ; j++, cur_exp = NULL) {
				
				tok[j] = strtok_r(cur_exp, " ", &saveptr2);
			
				if(tok[j] == NULL) break;
			}
			
		}	

	/*	for (int waits = 0; waits <= i; waits++) {
			// LOOK INTO THIS MORE 
			wait(0);
		}

		// close all open fds
		for( ; fds_open_index > 0; fds_open_index--) {
			close(fds_open[fds_open_index-1]);
		}
	*/
		// case for two expressions
		wait(0);
		close(fds_open[1]);
		wait(0);
		close(fds_open[0]);
		// print new prompt 
		write(0,"$",1);	
	}
}

void run(char *program[], int in, int out) {
	pid_t child;
	//	printf("reading from %d\n", in);
	//printf("writing to %d\n", out);
	child = fork();
	if (child == 0) {
		dup2(in, 0);
		dup2(out, 1);
		execvp(program[0],program);
		exit(42);
	}	
}





