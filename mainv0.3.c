/*
 *  Description: This is a light implementation of a Linux shell
 *
 *  Copyright (C) 2016 Jaime Arrocha
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "builtin.h"

/* function prototypes */
void sig_handler(int);
void comm_identi(struct proc_st *, char *);

/* struct handling built-in commands */
static const struct builtin {
	const char *cmd;
	int (*func)(struct proc_st *);
} cmds[] = {
	{ "exit", cmd_exit},
	{ "cd", cmd_cd},
	{ 0 }
};


int main()
{
	struct proc_st *proc;
	char buff[MAX_LEN];
	pid_t pid;
	int status;
	
	/* initialize signal handlers */
	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD, sig_handler);
	
	proc = malloc(sizeof(struct proc_st) + MAX_ARGS);
	
	/* start of shell loop */
	printf("[SHELL]> ");
	for (;;) {
		while (fgets(buff, MAX_LEN, stdin) != NULL) {

			/* initialize to zero for every command */
			memset(proc, 0, sizeof(struct proc_st));
			proc->exec = 1;
			comm_identi(proc, buff);

			if (proc->exec == 1) {
				if ((pid = fork()) < 0 ) {
					error_msg("fork error");
				} else if (pid == 0)	/* child */
					if (execvp(*(proc->commv), 
						proc->commv) < 0 )
						error_msg("could not execute");

				/* parent */
				if (proc->bg == 0) {
					if ((pid = waitpid(pid, &status, 0)) 
							< 0)
						error_msg("waitpid error");
				} else
					printf("PID %d %s\n", pid, proc->pbuff);
			}
			printf("[SHELL]> ");
		}
	}
	free(proc);
	exit(0);
}

/* signal handler */
void sig_handler(int signal)
{
	pid_t pid;
	const char msg[] = "Child Terminated";

	while ((pid = waitpid(-1, NULL, WNOHANG | WUNTRACED)) > 0)
		write(STDOUT_FILENO, msg, sizeof(msg) - 1);

	if (errno != ECHILD)
		exit(EXIT_FAILURE);
}

/* parses and identifies commands */
void comm_identi(struct proc_st *proc, char *buffer)
{
	const struct builtin *ctmp;
	char *tmp;
	
	/* replace newline with null */
	if (buffer[strlen(buffer) - 1] == '\n')
		buffer[strlen(buffer) - 1] = 0;
	strncpy(proc->pbuff,buffer,MAX_LEN);

	tmp = strtok(buffer, " ");
	while (tmp != NULL) {
		proc->commv[(proc->commc)++] = tmp;
		tmp = strtok(NULL, " ");
	}

	proc->commv[proc->commc] = NULL;
	
	/* check for built-in functions */
	if (proc->commc >= 1) {
		for (ctmp = cmds; ctmp->cmd; ctmp++) {
			if (matches(proc->commv[0], ctmp->cmd) == 0)
				ctmp->func(proc);
		}
	
		/* check if it is a background process */
		if (*proc->commv[(proc->commc) - 1] == '&') {
			proc->bg = 1;
			proc->commv[(proc->commc) - 1] = NULL;
		}
	}
		
	return;
}

/* execute commands */

