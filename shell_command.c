#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/wait.h>
#include <dirent.h>

#include "shell_command.h"
//reminder: there can only be one main function!

/** IMPLEMENTING FEATURE 1:
-read a line at a time
-parse the line to separate the command line from arguments
-fork and exec the command
-parent process should wait until exec'd program exits
-read the next command
-implement exit and cd on your own, they cannot run thru forked child process
-hint 1: assume all items on command line are to be separated by single space
*/

//PARAMTERS: line - line to be inputted and fixed
//RETURN: pointer to beginning of line
//PURPOSE: fix formatting of input to faciliate easier testing... removing white spaces
char * fix_format(char * line){
  //get rid of pesky spaces at front
  while(*line == ' '){
    line++; }

  //get rid of pesky spaces at back
  char * back = line + strlen(line) - 1;
  while (line < back && (*back == ' ')){
      back--; }

  //place terminating null
  back[1] = 0;

  return line;
}

/** IMPLEMENTING FEATURE 2:
-read and separate multiple commands on one line with ;
-hint 2: when parsing the line, count number of tokens (individual pieces)
*/

//PARAMETERS: line - the line to be inputted, separator - what each command is to separated by
//RETURN: number of tokens in the line
//default: return 1 if the separator is not found... this means single command
//PURPOSE: count number of tokens - distinct commands that are separated by a separator - in a line
int count_num_tokens(char * line, char separator){
  int n = 1;
  char * temp = line;
  while ( (temp = strchr(temp, separator)) ){
    temp++;
    n++; }
  return n;
}

//PARAMETERS: line - the line to be inputted
//RETURN: array of tokens (commands) that are separated by the separator
//PURPOSE: to break apart an inputted line into its individual token components
char ** parse_line(char * line, char separator){

    int num_tokens = count_num_tokens(line, separator);
    char ** answer = (char **)calloc(num_tokens, sizeof(char *));

    //make a char array for the separator to be used with strsep
    char the_separator[2];
    the_separator[0] = separator; //singular character
    the_separator[1] = 0; //terminating null

    int i = 0;
    //allocate memory
    while (i < num_tokens){
      answer[i] = (char *)calloc(num_tokens, sizeof(char *));
      i++; }

    i = 0;
    //populate array
    while (i < num_tokens){
      //remember to input the ADDRESS of line
      answer[i] = strsep(&line, the_separator);
      i++; }

    //terminating NULL
    answer[num_tokens] = 0;
    return answer;
}

/** IMPLEMENTING FEATURE 3: implement simple redirecion using > and < */

//PARAMETERS: std - the file to be redirected, original - to store old value of std,
// file - the new file to be redirected to
//RETURN: the file descriptor of the newly redirected file
//PURPOSE: redirect file descriptor std to new file
int redirect(int std, int *original, char * file){
  //store original value of std
  *original = dup(std);

  //fix formatting
  file = fix_format(file);

  //create new file if nonexistent
  int fd = open(file, O_RDWR | O_CREAT, 0644);
  //add later: handle error if opening file fails

  //redirect
  dup2(fd, std);
  return fd;

}

//PARAMETERS: std - the file to be redirected, original - to be reverted back to
//fd - the file to be closed
//RETURN: N/A
//PURPOSE: redirect file of file descriptor std to original file
//literally straight from class notes
int redirect_back(int std, int original, int fd){
  dup2(original, std);
  close(fd); //close file fd
  return 0; }

//new and improved!
//PARAMTERS: line - line to be inputted and executed
//RETURN: N/A
//PURPOSE: execute the command(s) inputted
//Parse the line and separate each argument by a white space
//The parent forks a new process and waits for the child to finish
//to be impleneted later: handle exit and cd differently
void exec_command(char * line){

  //setup
  line = fix_format(line);
  int tokens = count_num_tokens(line, ' ');
  char ** command_array = parse_line(line, ' '); //parse line into an array of commands
  int command;
  int status;

  //fork and then exec the command
  int f;
  f = fork();
  if (f == 0){
    command = execvp(command_array[0], command_array);
    //deal with errors later
  }
  else{
    f = wait(&status); }
}

//PARAMETERS: line - line of commands to be executed
//RETURN: N/A
//PURPOSE: the meat and bones of the shell
//process shell input commands to be executed
//separate each individual command by ; and executes each
//handle redirection and implement pipes if applicable
void process_input(char * line){
  //setup
  int num_tokens = count_num_tokens(line, ';'); //remember, this is number of commands
  int in_fd = 0;
  int out_fd = 0;
  int in_copy = 0;
  int out_copy = 0;

  int i = 0;
  char copy[1024];
  char pipe_file[1024];
  char *command;
  char *temp;
  while (i < num_tokens){
      //separate each individual command by ;
      strncpy(copy, strsep(&line, ";"), sizeof(copy) );
      command = copy; //command to be processed

      //redirect stdin?
      if (strchr(copy, '<') != 0){ //if there exists < in the Input
        strsep(&command, "<"); //separate token <
        in_fd = redirect(STDIN_FILENO, &in_copy, command);
        //refer to commenets on redirect function
      }

      //redirect stdout?
      if (strchr(copy, '>') != 0){
        strsep(&command, ">");
        out_fd = redirect(STDOUT_FILENO, &out_copy, command); }

      //implement pipe? (limit to single pipe)
      if (strchr(copy, '|') != 0){
        //redirect first command to temp
        temp = strsep(&command, "|");
        strncpy(pipe_file, ".tempfile", sizeof(pipe_file) );
        out_fd = redirect(STDOUT_FILENO, &out_copy, pipe_file);
        exec_command(temp); //execute the command

        //revert stdout
        out_fd = redirect_back(STDOUT_FILENO, out_copy, out_fd);

        //redirect stdin to temp
        in_fd = redirect(STDIN_FILENO, &in_copy, pipe_file);
        exec_command(command);


        remove(".tempfile"); }

      //no implementation of pipes desired
      else{
        exec_command(copy);

        //if in_fd initialized, then revert back from its redirection
        if (in_fd){
          in_fd = redirect_back(STDIN_FILENO, in_copy, in_fd); }

        //if out_fd initialized, then revert back from its redirection
        if (out_fd){
          out_fd = redirect_back(STDOUT_FILENO, out_copy, out_fd); }

      }
  }
}

//where the fun begins
int main(){
  char input[1024];
  while (1){
    printf("Anthony and William's Excellent Shell:" );
    fgets(input, 1024, stdin);

    //insert terminating null into end of command
    *strchr(input, '\n') = 0;

    if (strncmp(input, "exit", sizeof(input)) != 0){
      process_input(input); }
    else{
      exit(0); //exit the shell if user typed in exit
    }
  }

}
