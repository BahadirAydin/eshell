#ifndef ESHELL_H
#define ESHELL_H

#include "execute.h"
#include "parser.h"
#include <array>
#include <iostream>
#include <vector>

namespace eshell {
auto run_pipelined_cmds(const pipeline &p) -> void;
auto run(const parsed_input &input, int in[2] = nullptr, bool repeater = false)
    -> void;
} // namespace eshell

#endif // ESHELL_H
