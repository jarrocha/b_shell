/*
Description: This is a light implementation of a Linux shell
*/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 4096
#define MAX_ARGS 256


struct proc_st {
	int bg;
	int exec;
	int commc;
	int fd_out;
	int fd_in;
	pid_t pid;
	char pbuff[MAX_LEN];
	char *commv[];
};


/* function prototypes */
void error_msg(char *);
void sig_handler(int);
void comm_identi(struct proc_st *, char *);

int main()
{
	struct proc_st *proc;
	char buff[MAX_LEN];
	pid_t pid;
	int status;
	
	/* initialize signal handlers */
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGCHLD, sig_handler);
	
	printf("[SHELL]> ");
	for (;;) {
		
		while (fgets(buff, MAX_LEN, stdin) != NULL) {

			proc = malloc(sizeof(struct proc_st) + MAX_ARGS);
			
			/*initialize to zero */
			memset(proc, 0, sizeof(struct proc_st));
			proc->exec = 1;
			comm_identi(proc, buff);

			if (proc->exec == 1) {
				if ((pid = fork()) < 0 ) {
					printf("fork error");
				} else if (pid == 0) { /* child */
					if (execvp(*(proc->commv), 
						proc->commv) < 0 )
						error_msg("could not execute");
					free(proc);
				}

				/* parent */
				if (proc->bg == 0) {
					if ((pid = waitpid(pid, &status, 0)) 
							< 0)
						error_msg("waitpid error");
				} else
					printf("PID %d %s\n", pid, proc->pbuff);
			}
			free(proc);
			printf("[SHELL]> ");
		}
	}
	exit(0);
}



void sig_handler(int signal)
{
	pid_t pid;

	while ((pid = waitpid(-1, NULL, WNOHANG | WUNTRACED)) > 0) {
		printf("Child Process [%d]: Terminated", (int)pid);
	}
	if (errno != ECHILD)
		error_msg("waitpid() error");
}

void error_msg(char * msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}


void comm_identi(struct proc_st *proc, char *buffer)
{
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
		if(!strcmp(proc->commv[0], "exit")) {
			free(proc);
			exit(EXIT_SUCCESS);
		}

		if(!strcmp(proc->commv[0], "cd")) {
			proc->exec = 0;
			if (chdir(proc->commv[1]) != 0)
				error_msg("chdir() error");
		}

		/* check if it is a background process */
		if (*proc->commv[(proc->commc) - 1] == '&') {
			proc->bg = 1;
			proc->commv[(proc->commc) - 1] = NULL;
		}
	}
	
	return;
}
