#include "../include/reader.h"

Reader::Reader(std::vector<std::string>& args) : args_(args) {}

void Reader.read() {
    std::cout << '>';

    std::string line;
    Lexer lexer;
    do {
        std::getline(std::cin, line);

        lexer.point_to(&line);
        auto [tokens, parse_status] = lexer.tokenize();
        if (ps == ParseStatus::KeepParsing) continue;

        Parser parser(tokens, args_);
        ParseStatus ps = parser.parse(tokens);

        ++count;

    } while (ps == ParseStatus::KeepParsing);
}
