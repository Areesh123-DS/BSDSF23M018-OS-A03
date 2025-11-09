#include "shell.h"
RedirectInfo cmd_info;
Job bg_jobs[MAX_BG_JOBS];
int bg_count=0;
Var *vars_head = NULL;

char* read_cmd(char* prompt, FILE* fp) {
    if (fp == stdin) { 
        char* cmdline = readline(prompt);
        if (cmdline && cmdline[0] != '\0')
            add_history(cmdline);
        return cmdline;
    }

    char* cmdline = malloc(MAX_LEN);
    if (!fgets(cmdline, MAX_LEN, fp)) {
        free(cmdline);
        return NULL;
    }
    return cmdline;
}
char** split_commands(char* line, int* count) {
    int capacity = 10;
    char** cmds = malloc(sizeof(char*) * capacity);
    *count = 0;

    char* start = line;
    char* cp = line;

    while (*cp != '\0') {
        if (*cp == ';') {
            int len = cp - start;
            cmds[*count] = (char*)malloc(len + 1);
            strncpy(cmds[*count], start, len);
            cmds[*count][len] = '\0';
            (*count)++;
            if (*count >= capacity) {
                capacity *= 2;
                cmds = realloc(cmds, sizeof(char*) * capacity);
            }
            cp++;
            while (*cp == ' ' || *cp == '\t') cp++; // skip spaces after ;
            start = cp;
        } else {
            cp++;
        }
    }

    // Add the last command
    if (cp != start) {
        int len = cp - start;
        cmds[*count] = (char*)malloc(len + 1);
        strncpy(cmds[*count], start, len);
        cmds[*count][len] = '\0';
        (*count)++;
    }

    return cmds;
}     
char** tokenize(char* cmdline) {
    static char* next_cmd_start = NULL;

    if (cmdline != NULL) next_cmd_start = cmdline;
    if (next_cmd_start == NULL || *next_cmd_start == '\0') return NULL;

    // Find next semicolon
    char* semicolon = next_cmd_start;
    while (*semicolon != '\0' && *semicolon != ';') semicolon++;

    // Calculate length of the single command
    int cmd_len = semicolon - next_cmd_start;

    while (cmd_len > 0 && (*next_cmd_start == ' ' || *next_cmd_start == '\t')) {
        next_cmd_start++;
        cmd_len--;
    }

    // Trim trailing spaces
    while (cmd_len > 0 && next_cmd_start[cmd_len - 1] == ' ') cmd_len--;

    // Copy the single command for parsing
    char* single_cmd = strndup(next_cmd_start, cmd_len);

    // Move pointer for next call
    next_cmd_start = (*semicolon == ';') ? semicolon + 1 : semicolon;
    cmd_info.is_background = 0;
    int arg_len = strlen(single_cmd);
    if (arg_len > 0 && single_cmd[arg_len - 1] == '&') {
        cmd_info.is_background = 1;
        single_cmd[arg_len - 1] = '\0'; 
    }
    else {
        cmd_info.is_background = 0;
    }

    // Edge case: empty command
    if (single_cmd == NULL || single_cmd[0] == '\0') return NULL;

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        memset(arglist[i], 0, ARGLEN);
    }

    cmd_info.input_file = NULL;
    cmd_info.output_file = NULL;
    cmd_info.pipe_cmd = NULL;
    cmd_info.has_input = cmd_info.has_output = cmd_info.has_pipe = 0;

    char* cp = single_cmd;
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
    
        
    arglist[argnum] = NULL;
    cmd_info.args = arglist;

    for (int i = 0; i < argnum; i++) {
        if (arglist[i][0] == '$' && arglist[i][1] != '\0') {
           
            char *val = get_var(arglist[i] + 1);
            free(arglist[i]);           
            if (val != NULL) {
                arglist[i] = strdup(val);   
            } else {
                arglist[i] = strdup("");    
            }
        }
    }

    if (argnum == 0) { // empty command
        for (int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        free(single_cmd);
        return NULL;
    }

    arglist[argnum] = NULL; // NULL-terminate the list
    cmd_info.args = arglist;
    free(single_cmd);

    return arglist;
}

void set_var(const char *name, const char *value) {
    if (name == NULL) return;

    Var *p = vars_head;

    while (p != NULL) {
        if (strcmp(p->name, name) == 0) {
            free(p->value);

            if (value != NULL)
                p->value = strdup(value);
            else
                p->value = strdup("");

            return;
        }
        p = p->next;
    }

    Var *n = malloc(sizeof(Var));
    if (n == NULL) return;

    n->name = strdup(name);

    if (value != NULL)
        n->value = strdup(value);
    else
        n->value = strdup("");

    n->next = vars_head;
    vars_head = n;
}

char *get_var(const char *name) {
    if (name == NULL) return NULL;

    Var *p = vars_head;
    while (p != NULL) {
        if (strcmp(p->name, name) == 0)
            return p->value;
        p = p->next;
    }
    return NULL;
}

void print_vars(void) {
    Var *p = vars_head;
    while (p != NULL) {
        printf("%s=%s\n", p->name, p->value);
        p = p->next;
    }
}
void free_vars(void) {
    Var *p = vars_head;
    while (p) {
        Var *next = p->next;
        free(p->name);
        free(p->value);
        free(p);
        p = next;
    }
    vars_head = NULL;
}


int handle_builtin(char** arglist){
    const char* path=arglist[1];
    char* helpc []={"cd : Helps to change directory",
        "exit : Used to terminate the shell gracefully",
        "jobs : Used to show all the background and stopped jobs ",
        "history : To display the command history",
        "set : To display the variables"};
    char* builtin []={"cd","jobs","help","exit","history","set"};
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
            else if (strcmp(arglist[0], "history") == 0) {
                for (int c = 0; c < curr_count; c++) {
                    printf("%d  %s\n", c + 1, history[c]);
                }
                return 1;
            }
            else if (strcmp(arglist[0], "set") == 0) {
                print_vars();
                return 1;
            }
            else if (strcmp(arglist[0], "jobs") == 0) {
                if (bg_count == 0) {
                    printf("No background jobs.\n");}   
                else {
                    for (int i = 0; i < bg_count; i++) {
                        char sign;
                        if (i == bg_count - 1) {
                            sign = '+';}
                        else {
                            sign = '-';
                        }
                        printf("[%d]%c  Running                 %s &\n", i + 1,sign, bg_jobs[i].cmd);
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}
        
