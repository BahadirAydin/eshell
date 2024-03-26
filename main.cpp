#include "eshell.h"
#include "parser.h"
#include <iostream>
#include <unistd.h>

#define PROMPT "/> "
auto print_prompt() -> void { std::cout << PROMPT; }

auto readstdin() -> char * {
    char *line = nullptr;
    size_t size = 0;
    // this is stdio.h getline, easier than cpp one which uses strings
    // so i don't have to convert it to char*
    if (getline(&line, &size, stdin) == -1) {
        return nullptr;
    }
    // pointer arithmetic is bad however this should be fine
    line[strcspn(line, "\n")] = 0; // remove trailing newline
    return line;
}

auto main() -> int {
    parsed_input input;
    char *line = nullptr;
    while (true) {
        print_prompt();
        // NOTE: i assumed no need to call parse_line
        // if line is null, it means EOF
        line = readstdin();
        if ((line == nullptr) || strcmp(line, "quit") == 0) {
            break;
        }
        std::flush(std::cout);
        parse_line(line, &input);
        // pretty_print(&input);
        int in = -1;
        eshell::run(input, in);
        free_parsed_input(&input);
    }
    return 0;
}
