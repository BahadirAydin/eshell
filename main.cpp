#include "eshell.h"
#include "parser.h"
#include <iostream>

auto readstdin(const char *prompt) -> char * {
    std::cout << prompt;
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
        line = readstdin("/> ");
        // NOTE: i assumed no need to call parse_line
        // if line is null, it means EOF
        if (!line || strcmp(line, "quit") == 0) {
            break;
        }
        int res = parse_line(line, &input);
        if (res == -1) {
            std::cout << "Error parsing input\n";
            exit(1);
        }
        // pretty_print(&input);
        eshell::run(input);
        free_parsed_input(&input);
    }
    return 0;
}
