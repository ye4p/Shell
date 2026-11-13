#include "../include/shell.hpp"

void Shell::loop() {
    int status;

    do {
        std::cout << '>';

        std::string line;
        Lexer lexer;
        Parser parser(args_);
        do {
            std::getline(std::cin, line);

            lexer.point_to(&line);
            auto [ps, tokens] = lexer.tokenize();

            for (const Token& t : tokens) {
                std::cout<< "Token: <" << t.content << "> of type " << static_cast<int>(t.tt) << '\n';
            }

            parser.point_to(&tokens);
            parser.cleanup();

            if (ps =! ParseStatus::KeepParsing)
                break;
        } while (true);

        cs = parser.parse();
        std::cout << cs;

        status = executeAllCommands();
    } while (status);
}

int Shell::launch(const std::vector<std::string>& args) {
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

int Shell::execute(const std::vector<std::string>& args) {
    if (args.empty())
        return 1; // Since no command was entered.

    for (size_t i = 0; i < num_builtins(); ++i) {
        if (args[0] == builtin_str_[i]) {
            return call_builtin(args, i);
        }
    }

    return launch(args);
}

int Shell::execute_all_commands() {
    int status;

    for (size_t i = 0; i < cs.vec.size(); ++i) {
        status = execute_logical_expression(&cs.vec[i]);
    }

    return status;
}

int Shell:execute_logical_expression(const LogicalExpression& le) {
    if (le.ops.size() + 1 != le.pipelines.size()) {
        throw std::runtime_error("Number of expressions and logical operators doesn't match\n");
    }

    if (le.ops.empty()) {
        return execute_pipeline(&le.pipelines[0]);
    }

    size_t i = 0;
    int status;
    while (i < le.pipelines.size()) {
        status = execute_pipeline(&le.pipelines[i]);
        if (status) { // That short circuits all logical OR
            while (i < le.ops.size() && le.ops[i] == LogicalOperator::Or) {
                ++i;
            }
            ++i;
        } else {
            while (i < le.ops.size() && le.ops[i] == LogicalOperator::And) {
                ++i;
            }
            ++i;
        }
    }

    return status;
}

int Shell::execute_pipeline(const Pipeline& p) {
    // For now ignore pipeline type
    
}

int Shell:execute_command(const Command& c) {
    std::string n;
    // Handle redirections
    
    // Handle process substitution.
    
    // Input process substitution needs to execute first. 
    // We need to get stdandard output of that command and 
    // direct it into standard input of main command
    for (size_t i = 0; i < c.ps.size(); ++i) {
        if (c.ps[i].st == SubstitutionType::Input) {
            in = execute_and_read(c.ps[i].) // need to define function for conversion
            break;
        }
    }
    std::string in = execute_and_read(
}

std::string Shell::execute_and_read(const std::vector<std::string>& args, int file_descriptor) {
    
}

std::vector<const char*> Shell::get_args(const std::vector<std::string>& args) {
    std::vector<const char*> temp;
    for (const auto& s : args) {
        temp.push_back(s.data());
    }
    temp.push_back(nullptr);

    return temp;
}

size_t Shell::num_builtins() {
    return builtin_func_.size();
}

int Shell::call_builtin(const std::vector<std::string>& args, size_t i) {
    return (this->*builtin_func_[i])(args);
}

//
// Builtin functions definitions
//

int Shell::cd(const std::vector<std::string>& args) {
    if (args.size() <= 1) {
        std::cerr << "tny: expected argument to \"cd\"\n";
    } else {
        if (chdir(args.at(1).c_str()) != 0) {
            perror("tny");
        }
    }

    return 1;
}

int Shell::help(const std::vector<std::string>& args) {
    std::cout << "DG's tny shell\n";
    std::cout << "Type command with its arguments and click enter\n";
    std::cout << "Built in programs:\n";
    for (std::string& s : builtin_str_) {
        std::cout << "  " << s << "\n";
    }

    std::cout << "use man page to find more information about commands\n";
    return 1;
}

int Shell::exit(const std::vector<std::string>& args) {
    return 0;
}

int Shell::pwd(const std::vector<std::string>& args) {
    std::cout << std::filesystem::current_path() << "\n";
    return 1;
}

int Shell::type(const std::vector<std::string>& args) {
    if (args.size() <= 1)
        std::cerr << "tny: expected argument to \"type\"\n";
    if (std::find(builtin_str_.begin(), builtin_str_.end(), args[1]) != builtin_str_.end()) 
        std::cout << "Shell builtin\n";
    else
        std::cout << "Not a shell builtin\n";
    return 1;
}
