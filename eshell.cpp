#include "eshell.h"
#include "execute.h"
#include "parser.h"

auto eshell::run_pipelined_cmds(const pipeline &p) -> void {
    int num_cmds = p.num_commands;
    std::vector<command> cmds(num_cmds);
    for (int i = 0; i < num_cmds; i++) {
        cmds[i] = p.commands[i];
    }
    execute::execute_pipeline_concurrent(cmds, true);
}

// determines the order of the commands from the parsed_input struct
auto eshell::run(parsed_input &input, int &in) -> void {
    int num_inputs = input.num_inputs;
    Pipelines parallel_plines;
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
            switch (input.separator) {
            case SEPARATOR_NONE: {
                run_pipelined_cmds(cmd.data.pline);
                break;
            }
            case SEPARATOR_PIPE: {
                std::cerr << "sep_pipe not implemented yet" << std::endl;
                break;
            }
            case SEPARATOR_SEQ: {
                run_pipelined_cmds(cmd.data.pline);
                break;
            }
            case SEPARATOR_PARA: {
                parallel_plines.push_back(cmd.data.pline);
                break;
            }
            }
            break;
        }
        case INPUT_TYPE_SUBSHELL: {
            execute::SubshellReturn subshell_return;
            if (!pipeline_cmds.empty() && i != num_inputs - 1) {
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    execute::failed_to_pipe();
                }
                execute::execute_pipeline_concurrent(pipeline_cmds, true, in,
                                                     pipefd[1]);
                close(pipefd[1]);
                pipeline_cmds.clear();
                subshell_return = execute::execute_subshell(cmd.data.subshell,
                                                            pipefd[0], false);
                if (subshell_return.type == execute::return_type::CHILD) {
                    in = -1;
                    run(subshell_return.input, in);
                    exit(0);
                } else {
                    in = subshell_return.in;
                }
                close(pipefd[0]);
            } else if (!pipeline_cmds.empty()) {
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    execute::failed_to_pipe();
                }
                execute::execute_pipeline_concurrent(pipeline_cmds, true, in,
                                                     pipefd[1]);
                close(pipefd[1]);
                pipeline_cmds.clear();
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, pipefd[0]);
                if (subshell_return.type == execute::return_type::CHILD) {
                    in = -1;
                    run(subshell_return.input, in);
                    exit(0);
                }
                close(pipefd[0]);
            } else if (i != num_inputs - 1) {
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, in, false);
                if (subshell_return.type == execute::return_type::CHILD) {
                    in = -1;
                    run(subshell_return.input, in);
                    exit(0);
                } else {
                    in = subshell_return.in;
                }
            } else {
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, in);
                if (subshell_return.type == execute::return_type::CHILD) {
                    in = -1;
                    run(subshell_return.input, in);
                    exit(0);
                } else {
                    in = subshell_return.in;
                }
            }
            break;
        }
        case INPUT_TYPE_NON: {
            // this is not used anywhere in the code idk what it means
            std::cerr << "unknown case" << std::endl;
            exit(1);
        }
        }
    }
    if (!pipeline_cmds.empty()) {
        if (in != -1) {
            execute::execute_pipeline_concurrent(pipeline_cmds, true, in);
        } else {
            execute::execute_pipeline_concurrent(pipeline_cmds, true);
        }
        pipeline_cmds.clear();
    }
    if (!parallel_cmds.empty()) {
        execute::execute_parallel(parallel_cmds);
        parallel_cmds.clear();
    }
    if (!parallel_plines.empty()) {
        // HACK: i am not really sure how and why flushing works
        // but it makes it so that i pass my blackbox test
        // it does not make any difference in interactive mode
        // std::flush(std::cout);
        execute::execute_parallel_pipelines(parallel_plines);
        parallel_plines.clear();
    }
}
