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
    if (child_pid != 0) { // PARENT PROCESS
        wait(nullptr);
    } else { // CHILD PROCESS
        execvp(data.args[0], data.args);
        failed_to_execute();
    }
}

// fd[0] -> read end
// fd[1] -> write end

auto execute::execute_pipeline(std::vector<command> &cmds) -> void {
    size_t n_cmds = cmds.size();
    // "in" is where the next command will read its input from
    int in = STDIN_FILENO;
    for (size_t i = 0; i < n_cmds; i++) {
        int fd[2];
        if (i < n_cmds - 1) {
            // no need to create a pipe for the last command
            // it will only read from the previous created
            // then peacefully die
            if (pipe(fd) < 0) {
                failed_to_pipe();
            }
        }
        pid_t child_pid = fork();
        if (child_pid > 0) { // PARENT PROCESS
            wait(nullptr);   // pipe processes are sequential, we wait and reap
            if (in != STDIN_FILENO) {
                // closes the read end of the pipe, each time this is called
                // a file descriptor is freed and the pipe is closed
                close(in);
            }
            if (i < n_cmds - 1) {
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
            if (i < n_cmds - 1) {
                // redirects the output to the write end of the pipe
                // if it is the last command, it will write to the stdout
                // since there is no more commands to read
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            execvp(cmds[i].args[0], cmds[i].args);
            failed_to_execute();
        }
    }
}

void execute::close_all_pipes(int pipes[][2], size_t n_pipes) {
    for (size_t i = 0; i < n_pipes; ++i) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

void execute::execute_pipeline_concurrent(std::vector<command> &cmds,
                                          bool _wait) {
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

auto execute::execute_parallel(std::vector<command> &cmds) -> void {
    size_t n_cmds = cmds.size();
    for (size_t i = 0; i < n_cmds; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) { // CHILD PROCESS
            execvp(cmds[i].args[0], cmds[i].args);
            failed_to_execute();
        }
    }
    for (size_t i = 0; i < n_cmds; i++) {
        wait(nullptr);
    }
}

auto execute::execute_parallel_pipelines(std::vector<pipeline> &plines)
    -> void {
    size_t n_plines = plines.size();
    for (size_t i = 0; i < n_plines; i++) {
        int n_cmds = plines[i].num_commands;
        std::vector<command> cmds(n_cmds);
        for (size_t j = 0; j < plines[i].num_commands; j++) {
            cmds[j] = plines[i].commands[j];
        }
        execute_pipeline_concurrent(cmds, false);
    }
    for (size_t i = 0; i < n_plines; i++) {
        for (size_t j = 0; j < plines[i].num_commands; j++) {
            wait(nullptr);
        }
    }
}
