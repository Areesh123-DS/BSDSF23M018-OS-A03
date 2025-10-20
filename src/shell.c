#include "shell.h"
RedirectInfo cmd_info;

char* read_cmd(char* prompt, FILE* fp) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s%s> ", prompt, strrchr(cwd, '/') ? strrchr(cwd, '/') + 1 : cwd);
    char* cmdline = (char*) malloc(sizeof(char) * MAX_LEN);
    int c, pos = 0;

    while ((c = getc(fp)) != EOF) {
        if (c == '\n') break;
        cmdline[pos++] = c;
    }

    if (c == EOF && pos == 0) {
        free(cmdline);
        return NULL; // Handle Ctrl+D
    }
    
    cmdline[pos] = '\0';
    return cmdline;
}

      

char** tokenize(char* cmdline) {
    // Edge case: empty command line
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n')
        return NULL;

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    cmd_info.input_file = NULL;
    cmd_info.output_file = NULL;
    cmd_info.pipe_cmd = NULL;
    cmd_info.has_input = cmd_info.has_output = cmd_info.has_pipe = 0;

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++; // skip spaces
        if (*cp == '\0') break;

        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
            len++;

        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';

        // Handle special tokens
        if (strcmp(arglist[argnum], "<") == 0) {
            cmd_info.has_input = 1;
            while (*cp == ' ' || *cp == '\t') cp++;
            start = cp;
            len = 0;
            while (*cp != '\0' && *cp != ' ' && *cp != '\t' && *cp != '\n') {
                cp++; len++;
            }
            cmd_info.input_file = strndup(start, len);
        } 
        else if (strcmp(arglist[argnum], ">") == 0) {
            cmd_info.has_output = 1;
            while (*cp == ' ' || *cp == '\t') cp++;
            start = cp;
            len = 0;
            while (*cp != '\0' && *cp != ' ' && *cp != '\t' && *cp != '\n') {
                cp++; len++;
            }
            cmd_info.output_file = strndup(start, len);
        } 
        else if (strcmp(arglist[argnum], "|") == 0) {
            cmd_info.has_pipe = 1;
            while (*cp == ' ' || *cp == '\t') cp++;
            start = cp;
            len = 0;
            while (*cp != '\0' && *cp != '\n') {
                cp++; len++;
            }
            cmd_info.pipe_cmd = strndup(start, len);
            break; // everything after pipe is in pipe_cmd
        } 
        else {
            argnum++; // only increment for normal arguments
        }
    }

    if (argnum == 0) { // empty command
        for (int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL; // NULL-terminate the list
    cmd_info.args = arglist;
    return arglist;
}

int handle_builtin(char** arglist){
    const char* path=arglist[1];
    char* helpc []={"cd : Helps to change directory",
        "exit : Used to terminate the shell gracefully",
        "jobs : Used to show all the background and stopped jobs ",
        "history : To display the command history"};
    char* builtin []={"cd","jobs","help","exit","history"};
    int builtin_size = sizeof(builtin) / sizeof(builtin[0]);
    for (int j = 0; j < builtin_size; j++) {
        if (strcmp(arglist[0],builtin[j])==0){
            if (strcmp(arglist[0],"cd")==0){
                if (path==NULL || strcmp(path,"~")==0){
                    path=getenv("HOME");
                }
                if (chdir(path)!=0){
                    perror("The system cannot find the path specified");   
                }
                return 1;
            }
            else if (strcmp(arglist[0],"exit")==0){
                    exit(0);}
            else if(strcmp(arglist[0],"help")==0){
                int size = sizeof(helpc) / sizeof(helpc[0]);
                for (int i=0; i<size;i++){
                    printf("%s\n",helpc[i]);
                }
                return 1;
            }
            else if (strcmp(arglist[0], "jobs") == 0) {
                printf("Job control not yet implemented.\n");
                return 1;
            }
            else if (strcmp(arglist[0], "history") == 0) {
                for (int c = 0; c < curr_count; c++) {
                    printf("%d  %s\n", c + 1, history[c]);
                }
                return 1;
            }
        }
    }
    return 0;
}
        
