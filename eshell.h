#ifndef ESHELL_H
#define ESHELL_H

#include "execute.h"
#include "parser.h"
#include <iostream>
#include <vector>

namespace eshell {
auto run(const parsed_input &input, bool repeater = false) -> void;
} // namespace eshell

#endif // ESHELL_H
