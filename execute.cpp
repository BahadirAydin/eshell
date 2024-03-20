#include "execute.h"

auto execute::failed_to_execute() -> void {
    std::cerr << "execute::failed to execute the command" << std::endl;
    exit(1);
}

auto execute::failed_to_pipe() -> void {
    std::cerr << "execute::failed to pipe" << std::endl;
    exit(1);
}

auto execute::execute_single_command(command data) -> void {

    pid_t child_pid = fork();
    if (child_pid) { // PARENT PROCESS
        wait(nullptr);
    } else { // CHILD PROCESS
        int status = execvp(data.args[0], data.args);
        if (status) {
            failed_to_execute();
        }
    }
}

// fd[0] -> read end
// fd[1] -> write end

auto execute::execute_pipeline(std::vector<command> &cmds) -> void {
    size_t num_cmds = cmds.size();
    // "in" is where the next command will read its input from
    int in = STDIN_FILENO;
    for (size_t i = 0; i < num_cmds; i++) {
        int fd[2];
        if (i < num_cmds - 1) {
            // no need to create a pipe for the last command
            // it will only read from the previous created
            // then peacefully die
            if (pipe(fd) < 0) {
                failed_to_pipe();
            }
        }
        pid_t child_pid = fork();
        if (child_pid) {   // PARENT PROCESS
            wait(nullptr); // pipe processes are sequential, we wait and reap
            if (in != STDIN_FILENO) {
                // closes the read end of the pipe, each time this is called
                // a file descriptor is freed and the pipe is closed
                close(in);
            }
            if (i < num_cmds - 1) {
                close(fd[1]); // closes the write end of the pipe
                in = fd[0];   // "in" is now the read end of the pipe, next
                              // command will read from it
            }
        } else { // CHILD PROCESS
            if (in != STDIN_FILENO) {
                dup2(in, STDIN_FILENO);
                close(in); // after redirecting the stdin to the pipe, we
                           // can close the original one
            }
            if (i < num_cmds - 1) {
                // redirects the output to the write end of the pipe
                // if it is the last command, it will write to the stdout
                // since there is no more commands to read
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            int status = execvp(cmds[i].args[0], cmds[i].args);
            if (status) {
                failed_to_execute();
            }
        }
    }
}

auto execute::execute_parallel(std::vector<command> &cmds) -> void {
    size_t num_cmds = cmds.size();
    for (size_t i = 0; i < num_cmds; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) { // CHILD PROCESS
            int status = execvp(cmds[i].args[0], cmds[i].args);
            if (status) {
                failed_to_execute();
            }
        }
    }
    for (size_t i = 0; i < num_cmds; i++) {
        wait(nullptr);
    }
}
