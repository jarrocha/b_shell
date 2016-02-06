This is an basic implementation of a Linux 2.6+ shell.
---

Updates
---
v2:
- better handling of signals. Removed unsafe function calls. But still needs
to sigaction().
- implemented jump table for built-in commands.
- removed unneeded allocation and frees.

TODO
-----
Implement:
- redirection
- job control, will require implementation of a regular linked list.
- update signal() to sigaction(), so as to handle the signals better.

Note: For the first two code re-structuring needs to be done
since some features were added in a rush.

- create function for command execution
- create function for command parsing
- create function for job create, deletion, and print.
