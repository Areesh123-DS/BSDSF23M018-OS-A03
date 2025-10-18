#include "shell.h"

char* history[HISTORY_SIZE];
int curr_count = 0;

int main() {
    char* cmdline;
    char** arglist;
    int replace_index=0;
    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {
        if (cmdline[0] == '!') {
            int n = atoi(cmdline + 1); 
            if (n <= 0 || n > curr_count) {
                printf("No such command exits in history\n");
                free(cmdline);
                continue;
            }
            free(cmdline); 
            cmdline = strdup(history[n - 1]);
            printf("%s\n", cmdline); 
        }

        if (curr_count < HISTORY_SIZE) {
            history[curr_count++] = strdup(cmdline);
        } else {
            free(history[replace_index]);
            history[replace_index] = strdup(cmdline);
            replace_index = (replace_index + 1) % HISTORY_SIZE;
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
    for (int i = 0; i < curr_count; i++) {
        free(history[i]);
    }

    return 0;
}
