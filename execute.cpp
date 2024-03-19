#include "execute.h"
#include "parser.h"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

auto execute::execute_single_command(command data) -> void {

    pid_t child_pid = fork();
    if (child_pid) { // PARENT PROCESS
        int *status = nullptr;
        wait(status);
    } else { // CHILD PROCESS
        int status = execvp(data.args[0], data.args);
        if (status) {
            std::cerr << "execute::failed to execute the command" << std::endl;
            exit(1);
        }
    }
}
