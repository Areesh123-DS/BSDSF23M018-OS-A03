#include "shell.h"

char* history[HISTORY_SIZE];
int curr_count = 0;

int main() {
    char* cmdline;
    char** arglist;
    int replace_index=0;
    
    while((cmdline = readline(PROMPT))!=NULL){
        if (cmdline[0]!='\0'){
            add_history(cmdline);
        }
        else{
            free(cmdline);
            continue;
        }
           
        if ((arglist = tokenize(cmdline)) != NULL) {

            if (handle_builtin(arglist)==0){
            execute(arglist);
            }

            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        free(cmdline);

    }

    printf("\nShell exited.\n");
    clear_history();

    return 0;
}
