#include "../include/reader.h"

Reader::Reader(std::vector<std::string>& args) : args_(args) {}

void Reader::read() {
    std::cout << '>';

    std::string line;
    Lexer lexer;
    do {
        std::getline(std::cin, line);

        lexer.point_to(&line);
        auto [tokens, parse_status] = lexer.tokenize();

        for (const Token& t : tokens) {
            std::cout<< "Token: <" << t.content << "> of type " << static_cast<int>(t.tt) << '\n';
        }

        if (parse_status != ParseStatus::KeepParsing) break;

        // Parser parser(tokens, args_);
        // ParseStatus ps = parser.parse(tokens);

    } while (true);
}
