#include <sys/wait.h>
#include <cunistd>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

class Shell {
private:
    std::vector<std::string> args;

    std::vector<std::string> builting_str = {
        "cd",
        "help",
        "exit",
    };

    int (*builtin_func[])() = {
        &cd,
        &help,
        &exit,
    };
    
public:
    void loop();
    std::string read_line() const;
    void split_line(std::string& line);
    int launch();
    int execute();

    int num_builtins();

    // Builtin commands:
    int cd();
    int help();
    int exit();
};
