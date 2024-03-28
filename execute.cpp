#include "execute.h"
#include "eshell.h"

auto execute::failed_to_execute() -> void {
    std::cerr << "execute::failed to execute the command" << std::endl;
    exit(1);
}

auto execute::failed_to_pipe() -> void {
    std::cerr << "execute::failed to pipe" << std::endl;
    exit(1);
}

auto execute::execute_single_command(command data, bool wait_) -> void {

    pid_t child_pid = fork();
    if (child_pid != 0) { // PARENT PROCESS
        if (wait_) {
            wait(nullptr);
        }
    } else { // CHILD PROCESS
        execvp(data.args[0], data.args);
        failed_to_execute();
    }
}

void execute::close_all_pipes(int pipes[][2], size_t n_pipes) {
    for (size_t i = 0; i < n_pipes; ++i) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

void execute::execute_pipeline(std::vector<command> &cmds, bool _wait,
                               int in_fd, int out_fd) {
    size_t n_cmds = cmds.size();
    size_t n_pipes = n_cmds - 1;
    if (n_cmds == 0) {
        return;
    }
    // create n-1 pipes for n commands
    int pipes[n_pipes][2];
    for (size_t i = 0; i < n_cmds - 1; ++i) {
        if (pipe(pipes[i]) < 0) {
            failed_to_pipe();
        }
    }
    for (size_t i = 0; i < n_cmds; ++i) {
        pid_t child_pid = fork();
        if (child_pid == 0) { // CHILD PROCESS
            if (i > 0) {
                // redirect input from the previous pipe
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < n_cmds - 1) {
                // redirect output to the next pipe
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            if (i == n_cmds - 1 && out_fd != -1) {
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            } else if (out_fd != -1) {
                close(out_fd);
            }
            if (i == 0 && in_fd != -1) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            } else if (in_fd != -1) {
                close(in_fd);
            }

            // we call this for every child because
            // each child has different file descriptors
            // and we need to close them
            close_all_pipes(pipes, n_cmds - 1);

            execvp(cmds[i].args[0], cmds[i].args);
            failed_to_execute();
        }
    }
    // this is for closing the pipes in the parent process
    close_all_pipes(pipes, n_cmds - 1);
    if (out_fd != -1) {
        close(out_fd);
    }
    // if i want to busy wait for the children to finish
    // i can call wait here by calling the function with _wait = true
    // but if i want to execute other commands in parallel
    // i call the function with _wait = false
    if (_wait) {
        for (size_t i = 0; i < n_cmds; ++i) {
            wait(nullptr);
        }
    }
}

auto execute::execute_parallel(std::vector<ParallelCommand> &plines)
    -> void {
    size_t n_plines = plines.size();
    for (size_t i = 0; i < n_plines; i++) {
        if (plines[i].type == SINGLE_INPUT_TYPE::INPUT_TYPE_COMMAND) {
            execute_single_command(plines[i].data.cmd, false);
            continue;
        }
        int n_cmds = plines[i].data.pline.num_commands;
        std::vector<command> cmds(n_cmds);
        for (size_t j = 0; j < n_cmds; j++) {
            cmds[j] = plines[i].data.pline.commands[j];
        }
        execute_pipeline(cmds, false);
    }
    // wait for all the children in all parallel subprocesses to finish
    for (size_t i = 0; i < n_plines; i++) {
        if (plines[i].type == SINGLE_INPUT_TYPE::INPUT_TYPE_COMMAND) {
            wait(nullptr);
            continue;
        }
        int n_cmds = plines[i].data.pline.num_commands;
        for (size_t j = 0; j < n_cmds; j++) {
            wait(nullptr);
        }
        // i didn't do error handling here but only acceptable types are COMMAND
        // and PIPELINE other cases should not happen ever.
    }
}

auto execute::execute_subshell(char *subshell, int in_fd, bool last)
    -> SubshellReturn {
    int pipefd[2];
    if (!last) {
        if (pipe(pipefd) == -1) {
            failed_to_pipe();
        }
    }
    pid_t child_pid = fork();
    if (child_pid == 0) { // CHILD PROCESS
        if (!last) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }
        if (in_fd != -1) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        parsed_input input;
        parse_line(subshell, &input);
        return SubshellReturn{input, return_type::CHILD, -1};
    }
    if (!last) {
        close(pipefd[1]);
    }
    wait(nullptr);
    return SubshellReturn{{}, return_type::PARENT, pipefd[0]};
}
