/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "readcmd.h"


int main()
{
	while (1) {
		struct cmdline *l;
		int i;
		int pid;
		int status;
		int fd_in, fd_out;
		int previous_pipe[2];
		int current_pipe[2];

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

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];

			if(l->seq[i+1] != 0){
				/* Il y a une commande suivante, il y a donc besoin d'un pipe */
			if(pipe(current_pipe) < 0){
					perror("shell: pipe creation failed ");
				}
			}

			/* Fork it !*/
			pid = fork();

			if(pid < 0){
				/* Processus erreur */
				perror("shell: fork failed ");
			}else if(pid == 0){
				/* Processus fils */
				if (i==0 && l->in != 0){
					/* Redirection de stdin */
					fd_in = open(l->in, O_RDONLY);
					if(fd_in < 0){
						/* Error on open */
						perror("shell: Error on open (stdin) ");
					}
					/* Remplace stdin */
					if(dup2(fd_in, STDIN_FILENO) < 0){
						/* Error on dup2 */
						perror("shell: Error on dup2 (stdin) ");
					}
				}

				if(i!=0){
					/* Il faut lire dans le pipe */
					/* Remplace stdin par previous_pipe[0] */
					close(previous_pipe[1]); /* Ferme le descripteur inutile dans le pipe */
					if(dup2(previous_pipe[0], STDIN_FILENO) < 0){
						/* Error on dup2 */
						perror("shell: Error on dup2 (stdin) ");
					}
				}

				if(l->seq[i+1] != 0){
					/* Il faut écrire dans le pipe */
					/* Remplace stdout par current_pipe[1] */
					close(current_pipe[0]); /* Ferme le descripteur inutile dans le pipe */
					if(dup2(current_pipe[1], STDOUT_FILENO) < 0){
						/* Error on dup2 */
						perror("shell: Error on dup2 (stdin) ");
					}
				}

				if (l->seq[i+1] == 0 && l->out != 0){
					/* Redirection de stdout */
					fd_out = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					if(fd_out < 0){
						/* Error on open */
						perror("shell: Error on open (stdout) ");
					}
					/* Remplace stdout */
					if(dup2(fd_out, STDOUT_FILENO) < 0){
						/* Error on dup2 */
						perror("shell: Error on dup2 (stdout) ");
					}
				}

				if(execvp(cmd[0], cmd) == -1){
					/* Error on execvp */
					perror("shell: error on execvp ");
					exit(-1);
				}
			}else{
				if(i > 0){
					/* Ferme le stdout du pipe précédent */
					close(previous_pipe[0]);
				}
				if(l->seq[i+1] != 0){
					/* ferme le stdin du pipe courant */
					close(current_pipe[1]);
				}
				/* Processus père */
				waitpid(pid, &status, 0);

				/* Change les descripteurs courant en descripteur précédents  */
				previous_pipe[0] = current_pipe[0];
				previous_pipe[1] = current_pipe[1];

			}
		}
	}
}
