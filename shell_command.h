void exec_command(char * line);
int count_num_tokens(char * line, char separator);
char ** parse_line(char * line, char separator);
char * fix_format(char * line);
int redirect(int std, int *original, char * file);
void process_input(char * line);
