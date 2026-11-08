#include "../include/shell.h"
#include "../include/reader.h"

void Shell::loop() {
    int status;
    Line line;

    do {
        Reader r(args_);
        r.read();

        for (const std::string& s : args_) {
            std::cout << ">>" << s << "<<\n";
        }
        status = execute();
    } while (status);
}

void Shell::split_line(Line* line) {
    args_.clear();
    Parser p(line, args_);
    p.parse();
}

int Shell::launch() {
    pid_t pid; // Removed wpid cause its unnecessary at the moment
    int status;
    pid = fork();

    if (pid == 0) {
        // Child process:
        // Construct a vector of char*:
        std::vector<char*> temp;
        for (auto& arg : args_) {
            temp.push_back(arg.data());
        }
        if (execvp(temp[0], temp.data()) == -1)
            perror("tny");
        std::exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking:
        perror("tny");
    } else {
        // Parent process:
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int Shell::execute() {
    if (args_.empty())
        return 1; // Since no command was entered.

    for (size_t i = 0; i < num_builtins(); ++i) {
        if (args_[0] == builtin_str_[i]) {
            return call_builtin(i);
        }
    }

    return launch();
}

size_t Shell::num_builtins() {
    return builtin_func_.size();
}

int Shell::call_builtin(size_t i) {
    return (this->*builtin_func_[i])();
}

//
// Builtin functions definitions
//

int Shell::cd() {
    if (args_.size() <= 1) {
        std::cerr << "tny: expected argument to \"cd\"\n";
    } else {
        if (chdir(args_.at(1).c_str()) != 0) {
            perror("tny");
        }
    }

    return 1;
}

int Shell::help() {
    std::cout << "DG's tny shell\n";
    std::cout << "Type command with its arguments and click enter\n";
    std::cout << "Built in programs:\n";
    for (std::string& s : builtin_str_) {
        std::cout << "  " << s << "\n";
    }

    std::cout << "use man page to find more information about commands\n";
    return 1;
}

int Shell::exit() {
    return 0;
}

int Shell::pwd() {
    std::cout << std::filesystem::current_path() << "\n";
    return 1;
}

int Shell::type() {
    if (args_.size() <= 1)
        std::cerr << "tny: expected argument to \"type\"\n";
    if (std::find(builtin_str_.begin(), builtin_str_.end(), args_[1]) != builtin_str_.end()) 
        std::cout << "Shell builtin\n";
    else
        std::cout << "Not a shell builtin\n";
    return 1;
}
