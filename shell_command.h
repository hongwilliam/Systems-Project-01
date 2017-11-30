char ** parse_args(char * line);
void exec_command(char * line);
int count_num_tokens(char * line, char seperator);
char ** parse_line(char * line, char seperator);
char * fix_format(char * line);
int redirect(int std, int *original, char * file);
int redirect_back(int fd, int std, int original);
void process_input(char * line);
