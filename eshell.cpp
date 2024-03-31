#include "eshell.h"

namespace eshell {

auto run_pipelined_cmds(const pipeline &p) -> void {
    int num_cmds = p.num_commands;
    std::vector<command> cmds(num_cmds);
    for (int i = 0; i < num_cmds; i++) {
        cmds[i] = p.commands[i];
    }
    execute::execute_pipeline(cmds, true);
}

auto run(const parsed_input &input, int in[2], bool repeater) -> void {
    int num_inputs = input.num_inputs;
    std::vector<command> pipeline_cmds;
    std::vector<execute::ParallelCommand> parallel_cmds;

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
                parallel_cmds.push_back({INPUT_TYPE_COMMAND, cmd.data});
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
                parallel_cmds.push_back({INPUT_TYPE_PIPELINE, cmd.data});
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
                execute::execute_pipeline(pipeline_cmds, true, in, pipefd);
                close(pipefd[1]);
                pipeline_cmds.clear();
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, pipefd, false);
                if (subshell_return.type == execute::return_type::CHILD) {
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
                execute::execute_pipeline(pipeline_cmds, true, in, pipefd);
                close(pipefd[1]);
                pipeline_cmds.clear();
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, pipefd);
                if (subshell_return.type == execute::return_type::CHILD) {
                    run(subshell_return.input, in, true);
                    exit(0);
                }
                close(pipefd[0]);
            } else if (i != num_inputs - 1) {
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, in, false);
                if (subshell_return.type == execute::return_type::CHILD) {
                    run(subshell_return.input, in, true);
                    exit(0);
                } else {
                    in = subshell_return.in;
                }
            } else {
                subshell_return =
                    execute::execute_subshell(cmd.data.subshell, in);
                if (subshell_return.type == execute::return_type::CHILD) {
                    run(subshell_return.input, in, true);
                    exit(0);
                } else {
                    in = subshell_return.in;
                }
            }
            break;
        }
        case INPUT_TYPE_NON: {
            std::cerr << "unknown case" << std::endl;
            exit(1);
        }
        }
    }
    if (!pipeline_cmds.empty()) {
        execute::execute_pipeline(pipeline_cmds, true, in);
        pipeline_cmds.clear();
    }
    if (!parallel_cmds.empty()) {
        if (repeater) {
            size_t n_cmds = parallel_cmds.size();
            if (fork() == 0) { // CHILD REPEATER PROCESS
                execute::execute_parallel(parallel_cmds, true);
            } else { // PARENT SUBSHELL PROCESS
                wait(nullptr);
            }
        } else {
            execute::execute_parallel(parallel_cmds);
        }
        parallel_cmds.clear();
    }
}
} // namespace eshell
