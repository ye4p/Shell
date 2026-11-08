#include "../include/reader.h"

Reader::Reader(std::vector<std::string>& args) : args_(args) {}

void Reader.read() {
    std::cout << '>';

    std::string line;
    do {
        std::getline(std::cin, line);

        Lexer lexer(line);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens, args_);
        ParseStatus ps = parser.parse(tokens);

    } while (ps == ParseStatus::KeepParsing);
}
