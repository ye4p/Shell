#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <algorithm>
#include <stdexcept>

#include "parser.hpp"
#include "node.hpp"


class Shell {
private:
    // With new parser, we might not need to store args here as each command already stores its own arguments
    // std::vector<std::string> args_;

    CommandSeparator cs;

    std::vector<std::string> builtin_str_ = {
        "cd",
        "help",
        "exit",
        "pwd",
        "type",
    };

    using func = int (Shell::*)();

    // int (*builtin_func[])() = {
    std::array<func, 5> builtin_func_ {
        &Shell::cd,
        &Shell::help,
        &Shell::exit,
        &Shell::pwd,
        &Shell::type,
    };

public:
    void loop();
    std::string read_line() const;
    void split_line(std::string& line);
    // int launch();
    // int execute();
    int executeAllCommands();


    size_t num_builtins();
    int call_builtin(size_t i);

    // Builtin commands:
    int cd();
    int help();
    int exit();
    int pwd();
    int type();
};
