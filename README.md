# Systems-Project-01
Shell project

by Anthony Hom and William Hong

Features: 
	Executes single commands while removing white space
	Parses through multiple commands in a line and counts individual inputs
	Implements redirection using < and > 
	Implements exits through a forked child process


Attempted: 
	Originally tried to implement pipes, but didn't account for errno. 
	Tried figuring out how "cd" is handled by "chdir"

Bugs:
	Running two commands in one line is not working. Only the first command is run, ignoring the second command, and exiting.

Files and Function Headers: 

