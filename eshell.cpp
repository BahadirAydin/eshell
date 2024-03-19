#include "eshell.h"
#include "execute.h"
#include "parser.h"

// determines the order of the commands from the parsed_input struct
auto eshell::run(parsed_input &input) -> void {
    int num_inputs = input.num_inputs;
    for (int i = 0; i < num_inputs; i++) {
        single_input cmd = input.inputs[i];
        if (cmd.type == INPUT_TYPE_NON) {
            // this is not used anywhere in the code idk what it means
            std::cerr << "unknown input type, terminating\n";
            exit(1);
        } else if (cmd.type == INPUT_TYPE_SUBSHELL) {
            std::cerr << "subshell not implemented\n";
        } else if (cmd.type == INPUT_TYPE_COMMAND) {
            execute::execute_single_command(cmd.data.cmd);
        } else if (cmd.type == INPUT_TYPE_PIPELINE) {
            std::cerr << "pipeline not implemented\n";
            exit(1);
        }
    }
}
