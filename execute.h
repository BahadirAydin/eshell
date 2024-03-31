#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <array>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace execute {

auto failed_to_execute() -> void;
auto failed_to_pipe() -> void;
auto failed_to_fork() -> void;
auto close_all_pipes(const std::vector<std::array<int, 2>> &pipes) -> void;
auto wait_all(const std::vector<pid_t> &pids) -> void;
auto to_stages(const single_input &input) -> std::vector<single_input>;
auto execute_pipeline(const std::vector<single_input> &stages, int in_fd = -1)
    -> std::vector<pid_t>;
auto execute_parallel(const std::vector<single_input> &inputs,
                      bool repeater = false) -> void;
auto execute_repeater(const std::vector<int> &write_fds) -> pid_t;
auto execute_subshell(const char *subshell, bool repeater) -> void;
} // namespace execute

#endif // EXECUTE_H
