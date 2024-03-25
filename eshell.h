#ifndef ESHELL_H
#define ESHELL_H

#include "execute.h"
#include "parser.h"
#include <array>
#include <iostream>
#include <optional>
#include <vector>

namespace eshell {
using Pipelines = std::vector<pipeline>;

auto run_pipelined_cmds(const pipeline &p) -> void;
auto run(parsed_input &input, int &in) -> void;
} // namespace eshell

#endif // ESHELL_H
