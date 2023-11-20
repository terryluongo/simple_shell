start with example inputs 
talk about collection of syscalls that have to happen to achieve result

while loop for fgets
each iteration processes one character

## output redirection
can't redirect the fd - ls is hardcoded to write to desceriptor 1 
cant affect the fd

call dup2 

could redefine 1 to be the output file 


open
dup2 w open fd 
then exec

## input redirection

relatively similar to output redirection
dup2 from fd 0

handle case where user is redirecting input and output

## piping

pipe syscall

ls | sort 

need one process on each side of the pipe 

2 forks then

shell fork and process fork 

one execs ls one exec 

pipe creates pair of fds

write to 2nd file descriptor, read from 1st?

in the ls one 

dup the write fd to the stdout

dup the read fd to stdin

waiting is implicit in read syscall
read is blocking syscall, waits for data to be available

how to original process wait?
if it only waitso n immedaite child, will prematurely print prompt

what if sort takes a really long time?

so the first child has to wait on the 
 

 so the child process wouldn't call fork, only the parent process ever calls fork 

have to figure out where pipe happens
