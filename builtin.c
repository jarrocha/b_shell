/*
 * bas_shell: builtin routine
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
int cmd_exit(struct proc_st *);
int cmd_cd(struct proc_st *); 
int matches(const char *, const char *);
void error_msg(const char *);

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

	if (proc->commv[1] == NULL) {
		if (chdir(getenv("HOME")) != 0)
			error_msg("chdir() HOME error");
	} else {
		if(chdir(proc->commv[1]) != 0)
			error_msg("chdir() HOME error");
	}
	
	return 0;
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
