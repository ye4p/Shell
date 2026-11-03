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
    size_t i = 0;

    int quotes_count = 0;
    char quote_type = '';
    bool quote_active = false;

    while (i < line.size()) {
        if (line[i] != ' ' && !quote) {
            ++i;
            continue;
        }

        if (line[i] == ''' || line[i] == '"') {
            if (quotes_count % 2 == 0) {
                quote_active = true;
            }
        }
        if (beg != i) {
            args.push_back(line.substr(beg, i - beg));
            beg = i + 1;
        }
        ++i;
    }
    if (line.back() != ' ')
        args.push_back(line.substr(beg));
}

int Shell::launch() {
    pid_t pid; // Removed wpid cause its unnecessary at the moment
    int status;
    pid = fork();

    if (pid == 0) {
        // Child process:
        // Construct a vector of char*:
        std::vector<char*> temp;
        for (auto& arg : args) {
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
    if (args.empty())
        return 1; // Since no command was entered.

    for (size_t i = 0; i < num_builtins(); ++i) {
        if (args[0] == builtin_str[i]) {
            return call_builtin(i);
        }
    }

    return launch();
}

size_t Shell::num_builtins() {
    return builtin_func.size();
}

int Shell::call_builtin(size_t i) {
    return (this->*builtin_func[i])();
}

//
// Builtin functions definitions
//

int Shell::cd() {
    if (args.size() <= 1) {
        std::cerr << "tny: expected argument to \"cd\"\n";
    } else {
        if (chdir(args.at(1).c_str()) != 0) {
            perror("tny");
        }
    }

    return 1;
}

int Shell::help() {
    std::cout << "DG's tny shell\n";
    std::cout << "Type command with its arguments and click enter\n";
    std::cout << "Built in programs:\n";
    for (std::string& s : builtin_str) {
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
    if (args.size() <= 1)
        std::cerr << "tny: expected argument to \"type\"\n";
    if (std::find(builtin_str.begin(), builtin_str.end(), args[1]) != builtin_str.end()) 
        std::cout << "Shell builtin\n";
    else
        std::cout << "Not a shell builtin\n";
    return 1;
}
