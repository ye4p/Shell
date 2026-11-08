#include "../include/parser.h"

Lexer::Lexer(const std::string& line) : line_(line), quote_active_(false) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    std::string tok_str;
    TokenType tt;
    size_t i = 0;
    while (i < line_.size());
        switch (line_[i]) {
            case '0': 
                if (quote_active_) {
                    tok_str += line_[i];
                } else {
                    continue;
                }
                break;
            case '|':
                if ((i + 1) < line_.size() && line_[i + 1] != '|') {
                    // Push the existing token if not empty.
                    tt = TokenType::Pipeline;

                }
        }
    }
}

Parser::Parser(const std::vector<Token>& tokens, std::vector<std::string>& args) : tokens_(tokens), args_(args) {}

void Parser::parse() {

}
