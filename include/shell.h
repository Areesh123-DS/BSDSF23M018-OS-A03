#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "AREESHA> "
#define HISTORY_SIZE 25
extern char* history[HISTORY_SIZE];
extern int curr_count;
typedef struct {
    char **args;        
    char *input_file;   
    char *output_file;  
    char *pipe_cmd;     
    int has_input;
    int has_output;
    int has_pipe;
} RedirectInfo;
extern RedirectInfo cmd_info;


// Function prototypes
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char** arglist);
int handle_builtin(char** arglist);

#endif // SHELL_H
