#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <algorithm>


class Shell {
private:
    std::vector<std::string> args;

    std::vector<std::string> builtin_str = {
        "cd",
        "help",
        "exit",
        "pwd",
        "type",
    };

    using func = int (Shell::*)();

    // int (*builtin_func[])() = {
    std::array<func, 5> builtin_func{
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
    int launch();
    int execute();

    size_t num_builtins();
    int call_builtin(size_t i);

    // Builtin commands:
    int cd();
    int help();
    int exit();
    int pwd();
    int type();
};
