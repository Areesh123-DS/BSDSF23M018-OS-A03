#include "shell.h"

char* history[HISTORY_SIZE];
int curr_count = 0;


int main() {
    char* cmdline;
    char** arglist;
    int replace_index=0;
    

    while (1) {
    
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            for (int i = 0; i < bg_count; i++) {
                if (bg_jobs[i].pid == pid) {
                    printf("\n[%d]+  Done                 %s\n", bg_jobs[i].job_num, bg_jobs[i].cmd);
                    free(bg_jobs[i].cmd);
            // Shift remaining jobs left
                for (int j = i; j < bg_count - 1; j++) {
                    bg_jobs[j] = bg_jobs[j + 1];
                }
                bg_count--;
                break;
            }
        }
    }

        cmdline = readline(PROMPT);
        if (cmdline == NULL) break;

        if (cmdline[0] != '\0') add_history(cmdline);
        else {
            free(cmdline);
            continue;
        }

        if (curr_count < HISTORY_SIZE)
            history[curr_count++] = strdup(cmdline);

        // Split line by semicolons
        int cmd_count;
        char** cmds = split_commands(cmdline, &cmd_count);

        for (int j = 0; j < cmd_count; j++) {
            char** arglist = tokenize(cmds[j]);
            if (arglist != NULL) {
                if (handle_builtin(arglist) == 0) {
                    execute(arglist);  // execute will now handle background
                }

                for (int i = 0; arglist[i] != NULL; i++)
                    free(arglist[i]);
                free(arglist);
            }
            free(cmds[j]);
        }

        free(cmds);
        free(cmdline);
    }

    printf("\nShell exited.\n");
    for (int i = 0; i < curr_count; i++)
        free(history[i]);
    clear_history();

    return 0;
}