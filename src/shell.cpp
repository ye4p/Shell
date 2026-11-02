#include "../include/shell.h"

void Shell::loop() {
    int status;
    std::string line;

    do {
        std::cout << "> ";
        line = read_line();
        split_line(line);
        status = execute();
    } while (status);
}

std::string Shell::read_line() const {
    std::string line;
    std::getline(std::cin, line);

    return line;
}

void Shell::split_line(std::string& line) {
    args.clear();
    size_t beg = 0;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] != ' ') continue;

        if (beg != i) {
            args.push_back(line.substr(beg, i - beg));
        }
    }
}

int Shell::launch() {
    pid_t pid, wpid;
    int status;
    pid = fork();

    if (pid == 0) {
        // Child process:
        if (execvp(args[0], args) == -1)
            perror("tny");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking:
        perror("tny");
    } else {
        // Parent process:
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int execute() {
    if (args.empty())
        return 1; // Since no command was entered.

    for (size_t i = 0; i < num_builtins; ++i) {
        if (args[0] == builtin_str[i]) {
            return (*builtin_func[i])(args);
        }
    }

    return launch();
}

int Shell::num_builtins() {
    return builtin_str.size();
}

//
// Builtin functions definitions
//

int Shell::cd() {
    if (args.size() <= 1) {
        std::cerr << "tny: expected argument to \"cd\"\n";
    } else {
        if (chdir(args.at(1)) != 0) {
            perror("tny");
        }
    }

    return 1;
}

int Shell:help() {
    std::cout << "DG's tny shell\n";
    std::cout << "Type command with its arguments and click enter\n");
    std::cout << "Built in programs:\n");
    for (std::string& s : builtin_str) {
        std::cout << "  " << s << "\n";
    }

    // Add message about man command
    return 1;
}

int Shell:exit() {
    return 0;
}
