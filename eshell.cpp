#include "eshell.h"

// determines the order of the commands from the parsed_input struct
auto eshell::run(parsed_input &input) -> void {
    int num_inputs = input.num_inputs;
    std::vector<command> cmds;

    for (int i = 0; i < num_inputs; i++) {
        single_input cmd = input.inputs[i];
        switch (cmd.type) {

        case INPUT_TYPE_COMMAND:
            switch (input.separator) {
            case SEPARATOR_NONE:
                execute::execute_single_command(cmd.data.cmd);
                break;
            case SEPARATOR_PIPE:
                cmds.push_back(cmd.data.cmd);
                break;
            case SEPARATOR_SEQ:
                break;
            case SEPARATOR_PARA:
                break;
            default:
                std::cerr << "unknown case" << std::endl;
                exit(1);
            }
            break;
        case INPUT_TYPE_PIPELINE:
            std::cerr << "Pipeline not implemented yet" << std::endl;
            break;
        case INPUT_TYPE_SUBSHELL:
            std::cerr << "Subshell not implemented yet" << std::endl;
            break;
        case INPUT_TYPE_NON:
            // this is not used anywhere in the code idk what it means
            std::cerr << "unknown case" << std::endl;
            exit(1);
        }
    }
    if (cmds.size() > 0) {
        execute::execute_pipeline(cmds);
    }
}
