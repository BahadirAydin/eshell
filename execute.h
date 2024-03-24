#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <iostream>
#include <optional>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace execute {
auto close_all_pipes(int pipes[][2], size_t n_pipes) -> void;
auto failed_to_execute() -> void;
auto failed_to_pipe() -> void;
auto execute_single_command(command data) -> void;
// warn: this execute_pipeline function is obsolete
// i'm using execute_pipeline_concurrent instead
// this is just here for reference
auto execute_pipeline(std::vector<command> &cmds) -> void;
auto execute_pipeline_concurrent(std::vector<command> &cmds, bool _wait,
                                 int in_fd = -1, int out_fd = -1) -> void;
auto execute_parallel(std::vector<command> &cmds) -> void;
auto execute_parallel_pipelines(std::vector<pipeline> &plines) -> void;
auto execute_subshell(
    char *subshell, int in_fd = -1, bool tie_to_stdout = false,
    std::optional<std::vector<command>> pipeline_cmds = std::nullopt)
    -> std::optional<parsed_input>;
} // namespace execute

#endif // EXECUTE_H
