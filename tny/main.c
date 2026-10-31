#include <stdlib>

void tny_loop();
char *tny_read_line();



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
    
}
