# Systems-Project-01
Shell project

by Anthony Hom and William Hong

## Features: 
	Executes single commands while removing white space
	Parses through multiple commands in a line and counts individual inputs
	Implements redirection using < and > 
	Implements exits through a forked child process


## Attempted: 
	Originally tried to implement pipes, but didn't account for errno. 
	Tried figuring out how "cd" is handled by "chdir"

## Bugs:
	Running multiple commands separated by ; works, but the shell immediately exits at the end due to seg fault

## Files and Function Headers: 
### shell_command.c
Main shell file
	Handles all line parsing functions, redirection, and forking of executing commands
	'''
	/*======== char * fix_format() ==========
	Inputs: (char * line)
	Returns: Pointer to beginning of line

	Removes whitespace in front and back of the string line. 
	Places a terminating null at a new location if necessary.
	====================*/

	/*======== int count_num_tokens() ==========
	Inputs: (char * line, char separator)
	Returns: Number of tokens in a line. If a separator is not found (single command), return 1. 

	Count the number of tokens, or distinct commands, that are separated by a separator - in a line.
	====================*/

	/*======== char ** parse_line() ==========
	Inputs: (char * line, char separator)
	Returns: Array of tokens (commands) that are separated by the separator

	Breaks apart an inputted line into its individual token components.
	If the line contains multiple tokens, each token is put into an array of commands. 
	====================*/

	/*======== int redirect() ==========
	Inputs: (int std, int *original, char * file)
	Returns: File descriptor of the newly redirected file

	Redirects file descriptor std to new file.
	====================*/

	/*======== int redirect_back() ==========
	Inputs: (int std, int original, int fd)
	Returns: None

	Redirects file of file descriptor std to original file
	====================*/

	/*======== void exec_command() ==========
	Inputs: (char * line)
	Returns: None

	Forks a child to execute the command(s) inputted
	Deals with any errors from execution afterwards
	Waits for child to finish before exiting
	====================*/

	/*======== void process_input() ==========
	Inputs: (char * line)
	Returns: None

	Process shell input commands to be executed
	Separates each individual command by ; and executes each
	Handles redirection of stdout and stdin and implements pipes if applicable
	====================*/











