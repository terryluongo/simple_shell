#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUF_MAX 4096

void run(char *program[], int in, int out);

int main(int argc, char *argv[]) {

	char buf[BUF_MAX];

	write(0, "$", 1);

	while(fgets(buf, BUF_MAX, stdin) != NULL) {

		buf[strlen(buf)-1] = '\0';

		char *tok[BUF_MAX];
		char *buf_copy, *cur_exp, *saveptr1, *saveptr2;
		buf_copy = buf;
		int i = 0, prev_read = 0, final_output_flag = 0;

		for(; ; i++, buf_copy = NULL) {
			
			int cur_pipe[2];
			
			cur_exp = strtok_r(buf_copy, "|", &saveptr1);
			
			// not the first call, we can call pipe if it is not null
			if (buf_copy == NULL) {

				// if NULL, no new pipe needed, just run previous exp.
				if (cur_exp == NULL) {
					if (final_output_flag) break;
					run(tok,prev_read,1);
				}

				// not the first and not NULL, need pipe
				else {
					if (pipe(cur_pipe) != 0) {
						printf("replace with perror");
						exit(0);
					}
					
					run(tok,prev_read,cur_pipe[1]);

					prev_read = cur_pipe[0];	
				}
			}

			if (cur_exp == NULL) break;

			int input_redir_flag = 0, output_redir_flag = 0, append_flag = 0, output_fd = 1;

			// inner token parsing loop
			for (int j = 0; ; j++, cur_exp = NULL) {
				
				tok[j] = strtok_r(cur_exp, " ", &saveptr2);
				
				// last token, done w/ parsing, have to break before strcmp
				if (tok[j] == NULL) {
					// if output redirection, no pipe so we run now
				 	if (final_output_flag) {
						run(tok,prev_read,output_fd);
					}
					
					break;      
				}	

				// input redirection handling 
				if (strcmp(tok[j],"<") == 0) {
					input_redir_flag = 1;
				}	
				else if (input_redir_flag) {
					if ((prev_read = open(tok[j], O_RDONLY)) == -1) {
					      perror("open");
					      exit(2);
					}	      
					j -= 2; // we don't want < or filename to be in args, so write over them
					input_redir_flag = 0;
				}

				//output redirection handling
				if (strcmp(tok[j],">") == 0) {
					output_redir_flag = 1;
				}
				else if (strcmp(tok[j],">>") == 0) {
					output_redir_flag = 1;
					append_flag = 1;
				}
				else if (output_redir_flag) {
					int flag = append_flag ? O_RDWR | O_CREAT | O_APPEND : O_RDWR | O_CREAT | O_TRUNC;

					if ((output_fd = open(tok[j], flag)) == -1) {
					      perror("open");
					      exit(2);
					}

					j -= 2;
					output_redir_flag = 0;
					final_output_flag = 1;	
				}
			}
		}	

		// wait up on all processes before new prompt
		for (int waits = 0; waits <= i; waits++) {
			wait(0);
		}

		// print new prompt 
		write(0,"$",1);	
	}
}

void run(char *program[], int in, int out) {
	pid_t child;
	child = fork();

	if (child == 0) {
		dup2(in, 0);
		dup2(out, 1);
		execvp(program[0], program);
		exit(42);
	}
	else {
		if (out != 1) {
			close(out);
		}
		if (in != 0) {
			close(in);
		}
	}
}





