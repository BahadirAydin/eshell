#include "execute.h"
#include "parser.h"
#include <iostream>
#include <vector>

using Pipelines = std::vector<pipeline>;

namespace eshell {
auto run_pipelined_cmds(const pipeline &p) -> void;
auto run(parsed_input &input) -> void;
} // namespace eshell
