#include "eshell.h"

// determines the order of the commands from the parsed_input struct
auto eshell::run(parsed_input &input) -> void {
    int num_inputs = input.num_inputs;
    std::vector<command> pipeline_cmds;
    std::vector<command> parallel_cmds;

    for (int i = 0; i < num_inputs; i++) {
        single_input cmd = input.inputs[i];
        switch (cmd.type) {

        case INPUT_TYPE_COMMAND: {
            switch (input.separator) {
            case SEPARATOR_NONE:
                execute::execute_single_command(cmd.data.cmd);
                break;
            case SEPARATOR_PIPE:
                pipeline_cmds.push_back(cmd.data.cmd);
                break;
            case SEPARATOR_SEQ:
                execute::execute_single_command(cmd.data.cmd);
                break;
            case SEPARATOR_PARA:
                parallel_cmds.push_back(cmd.data.cmd);
                break;
            }
            break;
        }
        case INPUT_TYPE_PIPELINE: {
            int num_cmds = cmd.data.pline.num_commands;
            std::vector<command> cmds;
            for (int j = 0; j < num_cmds; j++) {
                cmds.push_back(cmd.data.pline.commands[j]);
            }
            execute::execute_pipeline(cmds);
            break;
        }
        case INPUT_TYPE_SUBSHELL: {
            std::cerr << "Subshell not implemented yet" << std::endl;
            break;
        }
        case INPUT_TYPE_NON: {
            // this is not used anywhere in the code idk what it means
            std::cerr << "unknown case" << std::endl;
            exit(1);
        }
        }
    }
    if (pipeline_cmds.size() > 0) {
        execute::execute_pipeline(pipeline_cmds);
    }
    if (parallel_cmds.size() > 0) {
        execute::execute_parallel(parallel_cmds);
    }
}
