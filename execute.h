#include "parser.h"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace execute {
auto close_all_pipes(int pipes[][2], size_t num_pipes) -> void;
auto failed_to_execute() -> void;
auto failed_to_pipe() -> void;
auto execute_single_command(command data) -> void;
// warn: this execute_pipeline function is obsolete
// i'm using execute_pipeline_concurrent instead
// this is just here for reference
auto execute_pipeline(std::vector<command> &cmds) -> void;
auto execute_pipeline_concurrent(std::vector<command> &cmds, bool _wait)
    -> void;
auto execute_parallel(std::vector<command> &cmds) -> void;
auto execute_parallel_pipelines(std::vector<pipeline> &plines) -> void;
} // namespace execute
