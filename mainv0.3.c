/*
Description: This is a light implementation of a Linux shell
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
    
#define MAX_LEN 4096
static const int MAX_ARGS = 256;

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
int cmd_exit(struct proc_st *);
int cmd_cd(struct proc_st *);
int matches(const char *, const char *);
void print_comm(struct proc_st *);
void error_msg(const char *);
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
	
	/* initialize signal handlers */
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sa.sa_handler = sig_handler;
	sigaction(SIGCHLD, &sa, NULL);
	signal(SIGINT, SIG_IGN);

	proc = malloc(sizeof(struct proc_st) + MAX_ARGS);
	
	/* start of shell loop */
	printf("[SHELL]> ");
	for (;;) {
		fgets(buff, MAX_LEN, stdin);

		/* initialize to zero for every command */
		memset(proc, 0, sizeof(struct proc_st));
		proc->exec = 1;
		if (strlen(buff) == 1)
			proc->exec = 0;

		comm_identi(proc, buff);

		if (proc->exec == 1) {
			if ((pid = fork()) < 0 ) {
				error_msg("fork error");
			} else if (pid == 0) {	/* child */
				if (execvp(*(proc->commv), 
							proc->commv) < 0 )
					error_msg("could not execute");
			} else if (pid > 0) { /* parent */
				printf("Parenting\n");
				if (proc->bg == 0) {
					printf("Start waiting\n");
					if ((pid = waitpid(pid, NULL, 0)) < 0)
						error_msg("waitpid error");
					printf("Finished waiting\n");
				} else {
					printf("Leaving child alone\n");
					printf("PID %d %s\n", pid, proc->pbuff);
				}
			}
		}
		printf("[SHELL]> ");
	}
	free(proc);
	exit(0);
}

/* signal handler */
void sig_handler(int signal)
{
	int sverrno = errno;
	pid_t pid;
	const char msg[] = "Child Terminated";

	while ((pid = waitpid(-1, NULL, WNOHANG | WUNTRACED)) > 0)
		write(STDOUT_FILENO, msg, sizeof(msg) - 1);

	if (errno != ECHILD)
		exit(EXIT_FAILURE);

	errno = sverrno;
}

/* prints error messages */
void error_msg(const char * msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

/* compares input to builtin commands */
int matches(const char* cmd, const char* pattern)
{
	int len = strlen(cmd);

	if (len > strlen(pattern))
		return -1;
	return memcmp(pattern, cmd, len);
}

/* builtin exit */
int cmd_exit(struct proc_st *proc)
{
	free(proc);
	exit(EXIT_SUCCESS);
}

/* builtin cd */
int cmd_cd(struct proc_st *proc)
{
	proc->exec = 0;

	if ((proc->commv[1]) == NULL) {
		if (chdir(getenv("HOME")) != 0)
			error_msg("chdir() HOME error");
	} else if (chdir(proc->commv[1]) != 0)
		error_msg("chdir() error");
	return 0;
}

/* parses and identifies commands */
void comm_identi(struct proc_st *proc, char *buffer)
{
	const struct builtin *ctmp;
	char *tmp;
	
	/*
	if (strlen(buffer) == 1) {
		printf("size caught\n");
		if (strcmp(buffer, "\n\0")) {
			printf("Newline caught");
			proc->exec = 0;
			return;
		}
	}
	*/

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
			proc->commc--;
			print_comm(proc);
			//printf("Buffer after & mod: %s\n", proc->pbuff);
		}
	}
		
	return;
}

void print_comm(struct proc_st *proc)
{
	printf("Commc: %d\n", proc->commc);
	printf("Coomv: ");
	for (int i = 0; i < proc->commc ; i++)
	{
		printf(" %s", proc->commv[i]);
	}
	printf("\n");

}
