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
    while (line == nullptr || strcmp(line, "quit") != 0) {
        line = readstdin("/> ");
        int res = parse_line(line, &input);
        if (res == -1) {
            std::cout << "Error parsing input\n";
            exit(1);
        }
        // for debugging
        // pretty_print(&input);
    }
    return 0;
}
