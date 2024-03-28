#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <iostream>
#include <optional>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace execute {

enum return_type { CHILD, PARENT };

struct SubshellReturn {
    parsed_input input;
    return_type type;
    int in;
};
struct ParallelCommand {
    SINGLE_INPUT_TYPE type;
    single_input_union data;
};

auto close_all_pipes(int pipes[][2], size_t n_pipes) -> void;
auto failed_to_execute() -> void;
auto failed_to_pipe() -> void;
auto execute_single_command(command data, bool wait_ = true) -> void;
// warn: this execute_pipeline function is obsolete
// i'm using execute_pipeline_concurrent instead
// this is just here for reference
auto execute_pipeline(std::vector<command> &cmds, bool _wait, int in_fd = -1,
                      int out_fd = -1) -> void;
auto execute_parallel(std::vector<ParallelCommand> &plines) -> void;
auto execute_subshell(char *subshell, int in_fd = -1, bool last = true)
    -> SubshellReturn;
} // namespace execute

#endif // EXECUTE_H
