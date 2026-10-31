#include <stdlib.h>
#include <unistd.h>

#define TNY_RL_BUFSIZE 1024
#define TNY_TOK_BUFSIZE 64
#define TNY_TOK_DELIM " \t\r\n\a"

void tny_loop();
char *tny_read_line();
char **tny_split_line();
int tny_launch(char **args);



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
        args = tny_split_line();
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
            buffer[position++] = c;
        }

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

        token = strtok(line, TNY_TOK_DELIM);
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
        if (execv(args[0], args) == -1) {
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
