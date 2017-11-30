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
*/

//return array of tokens in the line
char ** parse_args(char * line){
  char **arguments = (char **)calloc(6, sizeof(char *));
  int i = 0;
  while(i < sizeof(arguments)){
    //hint 1: assume all items on command line are to be separated by single space
    arguments[i] = strsep(&line, " ");
    i++;
  }
  arguments[i] = NULL;
  return arguments;
}

/** IMPLEMENTING FEATURE 2:
-read and separate multiple commands on one line with ;
-hint 2: when parsing the line, count number of tokens (individual pieces)
*/

//tokens = distinct commands that are seperated by a seperator
//default: return 1 if the seperator is not found; this means single command
int count_num_tokens(char * line, char seperator){
  int n = 1;
  char * temp = line;
  while ( (temp = strchr(temp, seperator)) ){
    temp++;
    n++; }
  return n;
}

//return array of tokens (commands) that are seperated by the seperator
char ** parse_line(char * line, char seperator){

    int num_tokens = count_num_tokens(line, seperator);
    char ** answer = (char **)calloc(num_tokens, sizeof(char *));
    char the_seperator[2];
    the_seperator[0] = seperator; //singular character
    the_seperator[1] = 0; //terminating null

    int i = 0;
    //allocate memory
    while (i < num_tokens){
      answer[i] = (char *)malloc(512);
      i++; }

    i = 0;
    //populate array
    while (i < num_tokens){
      answer[i] = strsep(&line, the_seperator);
      i++; }

    //terminating NULL
    answer[num_tokens] = 0;
    return answer;
}

//return pointer to beginning of line
//fix formatting of input to faciliate easier testing
char * fix_format(char * line){

  //get rid of pesky spaces at front
  while(*line == ' '){
    line ++; }

  //get rid of pesky spaces at back
  char * back = line + strlen(line) - 1;
  while (line < back && (*back == ' ')){
      back --; }

  //place terminating null
  *(back + 1) = 0;

  return line;

}


/** IMPLEMENTING FEATURE 3:
-implement simple redirecion using > and <
*/

//redirect file descriptor std to new file
//create new file if nonexistent
//return file descriptor for redirected file
int redirect(int std, int *original, char * file){
  //store original value of std
  *original = dup(std);

  //fix formatting
  file = fix_format(file);

  //create file
  int fd = open(file, O_RDWR | O_CREAT, 0644);

  //redirect
  dup2(fd, std);
  return fd;

}

//redirect file descriptor std to original file
//literally straight from class notes
int redirect_back(int std, int original, int fd){
  dup2(original, std);
  close(fd);
  return 0; }

//new and improved
//to be impleneted later: handle exit and cd differently
//each argument seperated by ' '
void exec_command(char * line){

  //setup
  line = fix_format(line);
  int tokens = count_num_tokens(line, ' ');
  char ** command_arr = parse_line(line, ' ');
  int command;
  int status;

  //fork and then exec the command
  int f;
  f = fork();
  if (f == 0){
    execvp(command_arr[0], command_arr);
    //deal with errors later
  }
  else{
    f = wait(&status); }
}

//process shell input commands to be executed
//seperate each individual command by ; and executes each
//handle redirection and implement pipes if applicable
void process_input(char * line){
  //setup
  int num_tokens = count_num_tokens(line, ';'); //this is number of commands
  int in_fd = 0;
  int out_fd = 0;
  int in_copy = 0;
  int out_copy = 0;

  int i = 0;
  char copy[512];
  char pipe_file[512];
  char *s;
  char *temp;
  while (i < num_tokens){
      //seperate each individual command by ;
      strncpy(copy, strsep(&line, ";"), sizeof(copy) );
      s = copy;

      //redirect stdin?
      if (strchr(s, '<') != 0){ //if there exists < in the Input
        strsep(&s, "<"); //seperate token <
        in_fd = redirect(STDIN_FILENO, &in_copy, s);
        //refer to commenets on redirect function
      }

      //redirect stdout?
      if (strchr(s, '>') != 0){
        strsep(&s, ">");
        out_fd = redirect(STDOUT_FILENO, &out_copy, s); }

      //implement pipe? (limit to single pipe)
      if (strchr(s, '|') != 0){
        //redirect first command to temp
        temp = strsep(&s, "|");
        strncpy(pipe_file, ".tempfile", sizeof(pipe_file) );
        out_fd = redirect(STDOUT_FILENO, &out_copy, pipe_file);
        exec_command(temp); //execute the command

        //redirect stdin to temp
        in_fd = redirect(STDIN_FILENO, &in_copy, pipe_file);
        exec_command(s);

        //revert stdout
        out_fd = redirect_back(STDOUT_FILENO, out_copy, out_fd);
        remove(".tempfile"); }

      //no implementation of pipes desired
      else{
        exec_command(s);

        //if in_fd initialized, then revert back from its redirection
        if (in_fd){
          in_fd = redirect_back(STDIN_FILENO, in_copy, in_fd); }

        //if out_fd initialized, then revert back from its redirection
        if (out_fd){
          out_fd = redirect_back(STDOUT_FILENO, out_copy, out_fd); }

      }
  }
}


int main(){
  //testing input
  char *s;
  printf("Input commands here: ");
  s = fgets(s, sizeof(s),stdin);
  printf("Inputted: %s", s); //immediate feedback

  int tokens = count_num_tokens(s, ' ');
  char ** args = parse_line(s, ' ');
  printf("Number of tokens: %d \n", tokens);

  int i = 0;
  while(i < tokens){
    printf("Command: %s \n", args[i]); //print out each command
    execvp(args[0], args);//execute each command
    i++;
  }


  //char line2[] = "ls ;man chdir";
  //printf("\nnumber of tokens in 'ls ; man chdir': %d \n", count_num_tokens(line2, ' '));
}
