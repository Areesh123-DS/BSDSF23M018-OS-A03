#include "shell.h"

int execute(char* arglist[]) {
    int status;

    if (cmd_info.has_pipe) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
            return -1;
        }
    pid_t left_pid = fork();
        if (left_pid == 0) {
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
            close(pipefd[0]); // Close read end
            close(pipefd[1]);

            execvp(cmd_info.args[0], cmd_info.args);
            perror("Left command failed");
            exit(1);
        }

    pid_t right_pid = fork();
    if (right_pid == 0) {
        // Right child → Reader
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[0]);
        close(pipefd[1]);

        char **right_args = tokenize(cmd_info.pipe_cmd);
        execvp(right_args[0], right_args);
        perror("Right command failed");
        exit(1);
    }

        // Parent closes both pipe ends
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both children to finish
    waitpid(left_pid, &status, 0);
    waitpid(right_pid, &status, 0);
    return 0;
}

    int cpid = fork();

    switch (cpid) {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:{
        if (cmd_info.has_input) {
            int in_fd = open(cmd_info.input_file, O_RDONLY);
            if (in_fd < 0) {
                perror("Input file open failed");
                exit(1);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (cmd_info.has_output) {
            int out_fd = open(cmd_info.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("Output file open failed");
                exit(1);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        } 
        execvp(arglist[0], arglist);
        perror("Command not found"); 
        exit(1);
    }
    default: // Parent process
        waitpid(cpid, &status, 0);
        // printf("Child pid:%d exited with status %d\n", cpid, status >> 8);
        return 0;
    }
}


