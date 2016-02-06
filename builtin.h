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

#ifndef _BUILTIN_H_
#define _BUILTIN_H_
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
int cmd_exit(struct proc_st *);
int cmd_cd(struct proc_st *); 
int matches(const char *, const char *);
void error_msg(const char *);

#endif
