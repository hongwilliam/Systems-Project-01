#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
//more headers might need to be included


/** FEATURE 1:
-read a line at a time
-parse the line to separate the command line from arguments
-fork and exec the command
-parent process should wait until exec'd program exits
-read the next command
*/

//hint 2: when parsing the line, count number of tokens (individual pieces)
//assume all items on command line are to be separated by single space

//return array of tokens in the line
char ** parse_args(char * line){
  char **arguments = (char **)calloc(6, sizeof(char *));
  int i = 0;
  while(i < sizeof(arguments)){
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

  char *s = line;
  char **args = parse_args(line);
  execvp(args[0], args);
}

int main(){
  char line[] = "ls -l -a";
  char *s2 = line;
  exec_command(line);

}
