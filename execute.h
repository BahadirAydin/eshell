#include "parser.h"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace execute {
auto failed_to_execute() -> void;
auto failed_to_pipe() -> void;
auto execute_single_command(command data) -> void;
auto execute_pipeline(std::vector<command> &cmds) -> void;
auto execute_parallel(std::vector<command> &cmds) -> void;
auto execute_parallel_pipelines(std::vector<pipeline> &plines) -> void;
} // namespace execute
