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
    arguments[i] = strsep(&line," ");
    i++;
  }
  arguments[i] = NULL;
  return arguments;
}

void exec_command(char * line){

  //fork and then exec the command
  int f;
  f = fork();
  //work on forking and sleeping later

  char *s = line;
  char **args = parse_args(line);
  execvp(args[0], args);
}

/** IMPLEMENTING FEATURE 2:
-read and separate multiple commands on one line with ;
-hint 2: when parsing the line, count number of tokens (individual pieces)
*/

//tokens = separate commands, ';' seperates each commands
//default: return 1 if no ';' is found
int count_num_tokens(char * line){
  int n = 1;
  char * temp = line;
  while ( (temp = strchr(temp, ';')) ){
    temp++;
    n++; }
  return n;
}

//return array of tokens (commands) seperated by ';'
char ** parse_line(char * line){
    char ** answer;
    int i = 0;
    int num_tokens = 0;

    num_tokens = count_num_tokens(line);
    answer = (char **)calloc(num_tokens, sizeof(char *));

    //allocate memory
    while (i < num_tokens){
      answer[i] = (char *)malloc(512);
      i++; }

    i = 0;
    //populate array
    while (i < num_tokens){
      answer[i] = strsep(&line,";");
      i++; }

    //terminating NULL
    answer[i] = 0;
    return answer;
}

//return pointer to beginning of line
//fix formatting of input to faciliate easier testing
char * fix_format(char * line){

  //get rid of pesky spaces at front
  while(isspace(*line)){
    line ++; }

  //get rid of pesky spaces at back
  char * back = line + strlen(line) - 1;
  while (line < back && isspace(*back) ){
      end --; }

  //place terminating null
  *(end + 1) = 0;

  return line;

}


/** IMPLEMENTING FEATURE 3:
-implement simple redirecion using > and <
*/

//redirect file descriptor std to new file
//create new file if nonexistent
//return fie descriptor for redirected file
int redirect(int std, int *original, char * file){
  //store original value of std
  *original = dup(std);

  //fix formatting
  file = fix_format(file);

  //create file
  int fd = open(file, O_RDWR | O_CREATE, 0644);

  //redirect
  dup2(fd, std);
  return fd;

}

//redirect file descriptor std to original file
//literally straight from class notes
void redirect_back(int fd, int std, int original){
  dup2(original, std);
  close(fd); }


int main(){
  char line2[] = "ls ; man chdir";
  printf("\nnumber of tokens in 'ls ; man chdir': %d \n", count_num_tokens(line2));

  char line[] = "ls -l -a";
  exec_command(line);
}
