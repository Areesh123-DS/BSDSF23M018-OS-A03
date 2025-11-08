#include "shell.h"

char* history[HISTORY_SIZE];
int curr_count = 0;


int main() {
    char* cmdline;

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
        if (strncmp(cmdline, "if", 2) == 0) {
            char* then_buff = malloc(4096); then_buff[0] = '\0';
            char* else_buff = malloc(4096); else_buff[0] = '\0';
            char* curr_block = NULL;

        cmd_info.is_if_block = 1;
        cmd_info.if_line = strdup(cmdline); // store if command

        while (1) {
            char* next_line = readline("> ");
            if (!next_line) break;

            if (strncmp(next_line, "then", 4) == 0) {
                curr_block = then_buff;
                continue;
            }
            if (strncmp(next_line, "else", 4) == 0) {
                curr_block = else_buff;
                continue;
            }
            if (strncmp(next_line, "fi", 2) == 0) break;

            if (curr_block) {
                strcat(curr_block, next_line);
                strcat(curr_block, ";");
            }
            free(next_line);
        }

        cmd_info.then_block = then_buff;
        cmd_info.else_block = else_buff;
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