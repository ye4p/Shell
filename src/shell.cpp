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

std::tuple<int, std::string> Shell::launch(const std::vector<std::string>& args, FileDescriptor fd) {
    pid_t pid; // Removed wpid cause its unnecessary at the moment
    int status;
    pid = fork();

    int out_pipe[2];
    int err_pipe[2];

    if (pipe(out_pipe) == -1)
        throw std::runtime_error("stdout pipe failed");

    if (pipe(err_pipe) == -1) {
        close(out_pipe[0]);
        close(out_pipe[1]);
        throw std::runtime_error("stderr pipe failed");
    }

    if (pid == 0) {
        // Child process:
        // Construct a vector of char*:
        char** argv = get_argv(args);

        close(out_pipe[0]);
        close(err_pipe[0]);

        if (dup2(out_pipe[1], STDOUT_FILENO) == -1 || dup2(err_pipe[1], STDERR_FILENO) == -1) {
            _exit(127);
        }

        close(out_pipe[1]);
        close(out_pipe[2]);

        if (execvp(argv[0], argv) == -1)
            perror("tny");
        std::exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking:
        perror("tny");
    } else {
        // Parent process:
        close(out_pipe[1]);
        close(err_pipe[1]);

        std::string stdout_data;
        std::string stderr_data;

        bool out_open = true;
        bool err_open = true;

        char buffer[4096];

        while (out_open || err_open) {
            fd_set fds;
            FD_ZERO(&fds);

            int maxfd = -1;

            if (out_open) {
                FD_SET(out_pipe[0], &fds);
                maxfd - out_pipe[0];
            }

            if (err_open) {
                FD_SET(err_pipe[0], &fds);
                if (err_pipe[0] > maxfd)
                    maxfd = err_pipe[0];
            }

            int ready = select(maxfd + 1, &fds, nullptr, nullptr, nullptr);

            if (ready == -1) {
                if (errno = EINTR) continue;
                perror("select");
                break;
            }

            if (out_open && FD_ISSET(out_pipe[0], &fds)) {
                ssize_t n = read(out_pipe[0], buffer, sizeof(buffer));

                if (n > 0) 
                //    std::cout.write(buffer, n);
                    stdout_data.append(buffer, n);
                else if (n == 0) {
                    close(out_pipe[0]);
                    out_open = false;
                }
            }

            if (err_open && FD_ISSET(err_pipe[0], &fds)) {
                ssize_t n = read(err_pipe[0], buffer, sizeof(buffer));

                if (n > 0) 
                //    std::cerr.write(buffer, n);
                    stderr_data.append(buffer, n);
                else if (n == 0) {
                    close(err_pipe[0]);
                    err_open = false;
                }
            }
        }

        if (out_open) close(out_pipe[0]);
        if (err_open) close(err_pipe[0]);

        std::string total;
        if (fd == FileDescriptor::StandardOutput || fd == FileDescriptor::Combined)
            total += stdout_data;
        if (fd == FileDescriptor::StandardError || fd == FileDescriptor::Combined)
            total += stderr_data;

        return {1, total};

        // do {
        //     waitpid(pid, &status, WUNTRACED);
        // } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

std::tuple<int, std::stiring> Shell::execute(const std::vector<std::string>& args, FileDescriptor fd) {
    if (args.empty())
        return {1, ""}; // Since no command was entered.

    for (size_t i = 0; i < num_builtins(); ++i) {
        if (args[0] == builtin_str_[i]) {
            return call_builtin(args, i, fd);
        }
    }

    return launch(args, fd);
}

int Shell::execute_all_commands() {
    int status = 1;

    for (size_t i = 0; i < cs.vec.size(); ++i) {
        status = execute_logical_expression(&cs.vec[i]);
    }

    return status;
}

int Shell::execute_logical_expression(const LogicalExpression& le) {
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

std::tuple<int, std::string> Shell::execute_command(const Command& c) {
    std::string in;
    std::string total;
    // Handle redirections
    
    // Handle process substitution.
    
    // Input process substitution needs to execute first. 
    // We need to get stdandard output of that command and 
    // direct it into standard input of main command
    for (size_t i = 0; i < c.ps.size(); ++i) {
        if (c.ps[i].st == SubstitutionType::Input) {
            auto [status, in] = execute_and_read(execute_and_read(c.ps[i].args), FileDescriptor::StandardOutput); // need to define function for conversion
            break;
        }
    }

    // Now if in exists it can be it should be fed into the program
    if (!in.empty());
        total += in;
    // The current problem is that we don't know which one was passed: redirection or substitution if both exist

}

std::string Shell::execute_and_read(const std::vector<std::string>& args, FileDescriptor fd) {
    
}

std::vector<const char*> Shell::get_argv(const std::vector<std::string>& args) {
    std::vector<const char*> temp;
    for (const auto& s : args) {
        temp.push_back(s.data());
    }
    temp.push_back(nullptr);

    return temp;
}

std::vector<const char*> Shell::get_argv(const std::vector<Word>& args) {
    std::vector<const char*> temp;
    for (const auto& s : args) {
        temp.push_back(s.word.data());
    }
    temp.push_back(nullptr);

    return temp;
}

size_t Shell::num_builtins() {
    return builtin_func_.size();
}

int Shell::call_builtin(const std::vector<std::string>& args, size_t i, FileDescriptor fd) {
    return (this->*builtin_func_[i])(args, fd);
}

//
// Builtin functions definitions
//

void Shell::print_and_append(std::ostream& out, std::string& str, const std::string& text, FileDescriptor fd) {
    cout << text;
    if (fd == FileDescriptor::Combined || (fd == FileDescriptor::StandardOutput && &out == &std::cout) || (fd == FileDescriptor::StandardError && &out == &std::cerr))
        str += text;
}


std::tuple<int, std::string> Shell::cd(const std::vector<std::string>& args, FileDescriptor fd) {
    std::string total;

    if (args.size() <= 1) {
        const char* err_txt = "tny: expected argument to \"cd\"\n";
        print_and_append(std::cerr, total, err_txt, fd);
    } else {
        if (chdir(args.at(1).c_str()) != 0) {
            perror("tny");
            if (fd == FileDescriptor::StandardError) 
                total += "tny";
        }
    }

    return {1, total};
}

std::tuple<int, std::string> Shell::help(const std::vector<std::string>& args, FileDescriptor fd) {
    std::string total;
    const char* main_msg = "DG's tny shell\nType command with its arguments and click enter\nBuilt in programs:\n";
    print_and_append(std::cout, total, main_mgs, fd);

    for (const std::string& s : builtin_str_) {
        const std::string temp = "  " + s + '\n';
        print_and_append(std::cout, total, temp, fd);
    }

    const char* man_msg = "use man page to find more information about commands\n";
    print_and_append(std::cout, total, man_msg, fd);

    return {1, total};
}

std::tuple<int, std::string> Shell::exit(const std::vector<std::string>& args, FileDescriptor fd) {
    return {0, ""};
}

std::tuple<int, std::string> Shell::pwd(const std::vector<std::string>& args, FileDescriptor fd) {
    std::string total;

    std::string path_msg =  std::filesystem::current_path() + '\n';
    print_and_append(std::cout, total, path_msg, fd);

    return {1, total};
}

std::tuple<int, std::string> Shell::type(const std::vector<std::string>& args, FileDescriptor fd) {
    std::string total;

    if (args.size() <= 1) {
        const char* err_msg = "tny: expected argument to \"type\"\n";
        print_and_append(std::cerr, total, err_msg, fd);
    }
    if (std::find(builtin_str_.begin(), builtin_str_.end(), args[1]) != builtin_str_.end()) {
        const char* msg1 = "Shell builtin\n";
        print_and_append(std::cout, total, msg1, fd);
    }
    else {
        const char* msg2 = "Not a shell builtin\n";
        print_and_append(std::cout, total, msg2, fd);
    }
    return {1, total};
}
