#include <sys/wait.h> //waitpid and other associated macros
#include <unistd.h> // chdir, fork, exec, pid_t etc...
#include <stdlib.h> // mallocs, free, exit codes...
#include <stdio.h> // perror, printf etc..
#include <string.h>

#define TNY_RL_BUFSIZE 1024
#define TNY_TOK_BUFSIZE 64
#define TNY_TOK_DELIM " \t\r\n\a"

void tny_loop();
char *tny_read_line();
char **tny_split_line(char*);
int tny_launch(char **args);


int tny_execute(char **args);

int main(int argc, char **argv) {
    // Load config files

    tny_loop();

    return EXIT_SUCCESS;
}

void tny_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = tny_read_line();
        args = tny_split_line(line);
        status = tny_execute(args);

        free(line);
        free(args);
    } while (status);
}

char *tny_read_line() {
    int bufsize = TNY_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);

    if (!buffer) {
        fprintf(stderr, "tny: Failed to allocate buffer memory\n");
        exit(EXIT_FAILURE);
    }

    int c;
    while (1) {
        c = getchar();

        if (c == '\n' || c == EOF) {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        ++position;

        if (position >= bufsize) { // Reallocate
            bufsize += TNY_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "tny: Failed to reallocate buffer memory\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **tny_split_line(char *line) { // In this version it ingores quotes completely
                                    // separates only by variations of whitespaces  
    int bufsize = TNY_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "tny: Failed to allocate buffer memory\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TNY_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= bufsize) {
            bufsize += TNY_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "tny: Failed to reallocate buffer memory\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TNY_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int tny_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("tny");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("tny");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

//
//  Function declarations for built in shell commands
//

int tny_cd(char** args);
int tny_help(char** args);
int tny_exit(char** args);

char* builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[]) (char**) = {
    &tny_cd,
    &tny_help,
    &tny_exit,
};

int tny_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int tny_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "tny: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("tny");
        }
    }

    return 1;
}

int tny_help(char **args) {
    printf("DG's tny\n");
    printf("Type command with its arguments and click enter\n");
    printf("Built in programs:\n");

    for (size_t i = 0; i < tny_num_builtins(); ++i) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use man command to see information about other programs\n");
    return 1;
}

int tny_exit(char **args) {
    return 0;
}

int tny_execute(char **args) {
    if (args[0] == NULL) {
        // Empty command
        return 1;
    }

    for (size_t i = 0; i < tny_num_builtins(); ++i) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return tny_launch(args);
}
