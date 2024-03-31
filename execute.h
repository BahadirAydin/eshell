#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace execute {

enum return_type { CHILD, PARENT };

struct SubshellReturn {
    parsed_input input;
    return_type type;
    int in[2];
};
struct ParallelCommand {
    SINGLE_INPUT_TYPE type;
    single_input_union data;
};

auto close_all_pipes(int pipes[][2], size_t n_pipes) -> void;
auto failed_to_execute() -> void;
auto failed_to_pipe() -> void;
auto execute_single_command(const command &data, bool wait_ = true,
                            int fd[2] = nullptr) -> void;
// warn: this execute_pipeline function is obsolete
// i'm using execute_pipeline_concurrent instead
// this is just here for reference
auto execute_pipeline(const std::vector<command> &cmds, bool _wait,
                      int in_fd[2] = nullptr, int out_fd[2] = nullptr) -> void;
auto execute_parallel(const std::vector<ParallelCommand> &parallel_cmds,
                      bool repeater = false) -> void;
auto execute_subshell(char *subshell, int in_fd[2] = nullptr, bool last = true)
    -> SubshellReturn;
} // namespace execute

#endif // EXECUTE_H
