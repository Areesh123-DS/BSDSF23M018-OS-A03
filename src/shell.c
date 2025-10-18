#include "shell.h"

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
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++; // Skip leading whitespace
        
        if (*cp == '\0') break; // Line was only whitespace

        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t')) {
            len++;
        }
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    if (argnum == 0) { // No arguments were parsed
        for(int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
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
        
