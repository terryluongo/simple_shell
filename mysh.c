#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUF_MAX 4096

void run(char *program[], int in, int out);

int run_prev_and_pipe(char *cur_exp, char **tok, int prev_read);

int handle_redirect(int redirect_flag, char *filename);

int detect_arrow(char *token);
	
int main(int argc, char *argv[]) {

	char buf[BUF_MAX];

	write(0, "$", 1);

	while(fgets(buf, BUF_MAX, stdin) != NULL) {

		buf[strlen(buf)-1] = '\0';
		char *tok[BUF_MAX];
		char *buf_copy, *cur_exp, *saveptr1, *saveptr2;
		buf_copy = buf;
		int i = 0, prev_read = 0;

		for(; ; i++, buf_copy = NULL) {
			
			cur_exp = strtok_r(buf_copy, "|", &saveptr1);

			// if not our first time, we run previous and then create pipe if necessary
			if (buf_copy == NULL) {
				prev_read = run_prev_and_pipe(cur_exp,tok,prev_read);
			}

			// stop if no more 
			if (cur_exp == NULL) break;

			if(strcmp(cur_exp, "exit\0") == 0) exit(0); 

			int redirect_flag = 0, fd = 0, output_fd = -1, inner_broken = 0;

			// inner token parsing loop
			for (int j = 0; ; j++, cur_exp = NULL) {

				tok[j] = strtok_r(cur_exp, " ", &saveptr2);
				
				// done w/ parsing, run now if no pipe
				if (tok[j] == NULL && output_fd >= 0) {
					// if output redirection, no pipe so we run now
					run(tok, prev_read, output_fd);
					inner_broken = 1;
					break;
				}	
				else if (tok[j] == NULL) {
					break;      
				}	

				fd = handle_redirect(redirect_flag, tok[j]);

				if (fd == -1) inner_broken = 1;
				
				if (redirect_flag > 0) j -= 2;

				if (redirect_flag > 1)	output_fd = fd;

				else if (redirect_flag == 1) prev_read = fd;

				redirect_flag = detect_arrow(tok[j]);
			
						
			}

			if (inner_broken) {
				break;
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

	if((child = fork()) == -1) {
		perror("fork");
		exit(320);
	}

	if (child == 0) {
		dup2(in, 0);
		dup2(out, 1);
		if(execvp(program[0], program) == -1) printf("mysh: %s: command not found", program[0]);
		exit(42);
	}
	else {
		if (out != 1) close(out);
		if (in != 0) close(in);
	}
}


int run_prev_and_pipe(char *cur_exp, char **tok, int prev_read) {

	int cur_pipe[2];
	int next_read;

	if (cur_exp == NULL) {
		run(tok,prev_read,1);
		next_read = -1;
	}

	// not the first and not NULL, need pipe
	else {
		if (pipe(cur_pipe) != 0) {
			perror("pipe");
			exit(0);
		}
		
		run(tok, prev_read, cur_pipe[1]);

		next_read = cur_pipe[0];	
	}
	return next_read;

}



int detect_arrow(char *token) {

	int return_flag = 0;

	if (strcmp(token, "<") == 0) return_flag = 1;
	
	else if (strcmp(token, ">") == 0) return_flag = 2;
	
	else if (strcmp(token, ">>") == 0) return_flag = 3;
	
	return return_flag;
}	

int handle_redirect(int redirect_flag, char *filename) {

	if (redirect_flag == 0) return -2;

	int fd, flag;

	if (redirect_flag == 1) flag = O_RDONLY;
	
	else if (redirect_flag == 2) flag = O_WRONLY | O_CREAT | O_TRUNC;
	
	else if (redirect_flag == 3) flag = O_WRONLY | O_CREAT | O_APPEND;

	int mode = (redirect_flag > 1) ? 0755 : 0;

	if (access(filename, (redirect_flag > 1) ? W_OK : F_OK) != 0) {
		if (redirect_flag == 1) printf("mysh: %s:  No such file or directory\n", filename);
		else printf("mysh: %s: Permission denied\n", filename);
	}

	if ((fd = open(filename, flag, mode)) == -1) {
		perror("open");
		exit(2);
	}

	return fd;

}
