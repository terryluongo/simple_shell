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


		for(i = 0; ; i++, buf_copy = NULL) {
			cur_exp = strtok_r(buf_copy, "|", &saveptr1);

			if (cur_exp == NULL) {
				run(NULL,prev_read,1);
				break;
			}

			int cur_pipe[2];
		        if (pipe(cur_pipe) != 0) {
				printf("replace with perror");
				exit(0);
			}	
			//printf("pipe read end: %d\n", cur_pipe[0]);
			//	printf("pipe write end: %d\n", cur_pipe[1]);
			fds_open[fds_open_index] = cur_pipe[0];
			fds_open[fds_open_index+1] = cur_pipe[1];
			fds_open_index += 2;
			// idea: pipe each time
			// run has two args, read and write 
			// on first run, has to be rd from stdin
			// on NULL run, has to be write to stdout
			// each non-null cur_exp creates new pipe 
			// save prev outside scope 
			// then submit the read of the prev and the write of new to run function

			char *tok[BUF_MAX];

			for (int j = 0; ; j++, cur_exp = NULL) {
				tok[j] = strtok_r(cur_exp, " ", &saveptr2);
			
				if(tok[j] == NULL) {
					run(tok,prev_read,cur_pipe[1]);
					break;
				}
			}
			//printf("this process reads from: %d\n", prev_read);
			//printf("this process writes to: %d\n", cur_pipe[1]);
			prev_read = cur_pipe[0];
			
		}	

		for (int waits = 0; waits <= i; waits++) {
			wait(0);
		}
		// CLOSE ALL PIPES AND FDS HERE
		for( ; fds_open_index > 0; fds_open_index--) {
			close(fds_open[fds_open_index-1]);
		}
		write(0,"$",1);	
	}
}

void run(char *program[], int in, int out) {
	pid_t child;
	printf("reading from %d\n", in);
	printf("writing to %d\n", out);
	child = fork();
	if (child == 0) {
		if (program == NULL) {
			printf("last time\n");
			char buf[BUF_MAX];
			ssize_t bytes_read;
			ssize_t bytes_written;
			/*while ((bytes_read = read(in, buf, BUF_MAX)) > 0) {
				bytes_written = write(out, buf, BUF_MAX);
				if(bytes_written == 0) {
					perror("write");
					exit(4);
				}
			} */
			//write(out, "hi how are you", 10);
			exit(2);

		}
		else {
		       printf("%s\n",program[0]);	
		dup2(in, 0);
		dup2(out, 1);
		execvp(program[0],program);
		exit(42);
		}
	}	
}





