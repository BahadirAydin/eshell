#include "execute.h"
#include "eshell.h"
#include <csignal>
#include <fcntl.h>
#include <string>

namespace execute {

auto failed_to_execute() -> void {
    std::cerr << "execute::failed to execute the command" << std::endl;
    exit(1);
}

auto failed_to_pipe() -> void {
    std::cerr << "execute::failed to pipe" << std::endl;
    exit(1);
}

auto failed_to_fork() -> void {
    std::cerr << "execute::failed to fork" << std::endl;
    exit(1);
}

auto close_all_pipes(const std::vector<std::array<int, 2>> &pipes) -> void {
    for (const auto &p : pipes) {
        close(p[0]);
        close(p[1]);
    }
}

auto wait_all(const std::vector<pid_t> &pids) -> void {
    for (pid_t pid : pids) {
        waitpid(pid, nullptr, 0);
    }
}

// a single command or subshell is just a pipeline with one stage
auto to_stages(const single_input &input) -> std::vector<single_input> {
    if (input.type != INPUT_TYPE_PIPELINE) {
        return {input};
    }
    std::vector<single_input> stages(input.data.pline.num_commands);
    for (size_t i = 0; i < stages.size(); ++i) {
        stages[i].type = INPUT_TYPE_COMMAND;
        stages[i].data.cmd = input.data.pline.commands[i];
    }
    return stages;
}

auto execute_pipeline(const std::vector<single_input> &stages, int in_fd)
    -> std::vector<pid_t> {
    size_t n_stages = stages.size();
    if (n_stages == 0) {
        return {};
    }
    std::vector<std::array<int, 2>> pipes(n_stages - 1);
    for (auto &p : pipes) {
        if (pipe(p.data()) == -1) {
            failed_to_pipe();
        }
    }
    // fork everything before waiting, otherwise a stage can block on a full
    // pipe that no one reads yet
    std::vector<pid_t> child_pids(n_stages);
    for (size_t i = 0; i < n_stages; ++i) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            failed_to_fork();
        }
        if (child_pid == 0) { // CHILD PROCESS
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            } else if (in_fd != -1) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < n_stages - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            close_all_pipes(pipes);

            if (stages[i].type == INPUT_TYPE_SUBSHELL) {
                // repeater is only needed if something is piped in
                execute_subshell(stages[i].data.subshell, i > 0 || in_fd != -1);
            }
            execvp(stages[i].data.cmd.args[0], stages[i].data.cmd.args);
            failed_to_execute();
        }
        child_pids[i] = child_pid;
    }
    close_all_pipes(pipes);
    return child_pids;
}

auto execute_parallel(const std::vector<single_input> &inputs, bool repeater)
    -> void {
    std::vector<pid_t> child_pids;
    std::vector<int> write_fds;
    for (const single_input &input : inputs) {
        int in_fd = -1;
        if (repeater) {
            int pipefd[2];
            // cloexec so other branches don't keep this pipe open
            if (pipe2(pipefd, O_CLOEXEC) == -1) {
                failed_to_pipe();
            }
            in_fd = pipefd[0];
            write_fds.push_back(pipefd[1]);
        }
        std::vector<pid_t> pids = execute_pipeline(to_stages(input), in_fd);
        child_pids.insert(child_pids.end(), pids.begin(), pids.end());
        if (in_fd != -1) {
            close(in_fd);
        }
    }
    if (repeater) {
        child_pids.push_back(execute_repeater(write_fds));
        for (int fd : write_fds) {
            close(fd);
        }
    }
    wait_all(child_pids);
}

auto execute_repeater(const std::vector<int> &write_fds) -> pid_t {
    pid_t child_pid = fork();
    if (child_pid == -1) {
        failed_to_fork();
    }
    if (child_pid != 0) { // PARENT PROCESS
        return child_pid;
    }
    // CHILD PROCESS
    // a branch may exit before reading everything, just stop writing to it
    signal(SIGPIPE, SIG_IGN);
    std::vector<int> fds = write_fds;
    char buf[4096];
    ssize_t bytesRead;
    while (!fds.empty() &&
           (bytesRead = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        for (size_t i = 0; i < fds.size();) {
            if (write(fds[i], buf, bytesRead) != bytesRead) {
                close(fds[i]);
                fds.erase(fds.begin() + i);
            } else {
                ++i;
            }
        }
    }
    exit(0);
}

auto execute_subshell(const char *subshell, bool repeater) -> void {
    std::string line(subshell);
    parsed_input input;
    parse_line(line.data(), &input);
    eshell::run(input, repeater);
    free_parsed_input(&input);
    exit(0);
}
} // namespace execute
