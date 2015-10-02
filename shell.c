/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "readcmd.h"


int main()
{
	while (1) {
		struct cmdline *l;
		int i;
		int pid;
		int status;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			pid = fork();

			if(pid < 0){
				perror("shell: fork failed\n");
			}else if(pid == 0){
				if(execvp(cmd[0], cmd) == -1){
					perror("shell: error on execvp\n");
					exit(-1);
				}
			}else{
				waitpid(pid, &status, 0);
			}
		}
	}
}
