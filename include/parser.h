#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

enum class ParseStatus {
    Inactive,
    Success,
    Fail,
    KeepParsing,
};

enum class TokenType {
    Uninitialized = -1, // Just for debugging purposes

    String = 0,
    SingleQuote = 1,
    DoublyQuote = 2,
    Pipeline = 3,       // |
    InputRedirection = 4,
    OutputRedirection = 5,    // > and other kinds
    ProcessSubstitutionBegin = 6, // Several things will have beginning like this, but the end will be just generic parenthese
    BackParentheses = 7,
    LogicalOperator = 8,
    Whitespace = 9,
};

// Contains basic information about token.
struct Token {
    std::string content;
    TokenType tt;
    Token(std::string content, TokenType tt) : content(content), tt(tt) {}
};

// Converts plain string into the list of tokens.
class Lexer {
    const std::string* line_;
    bool quote_active_;
    char quote_type_;

    std::string tok_str_;
    TokenType tt_;

    std::vector<Token> tokens_;
    size_t i;

public:
    Lexer();
    void point_to(const std::string* line);
    std::tuple<std::vector<Token>, ParseStatus> tokenize();
    char prev();
    char curr();
    char next();
    char at(size_t index);
    size_t size();
    bool is_oob(size_t index);
    void advance();
    void push_token();
    bool push_token_if_nonempty();
};

// Builds the AST tree and parses the tokens.
class Parser {
    const std::vector<Token>& tokens_;
    std::vector<std::string>& args_;

public:
    Parser(const std::vector<Token>& tokens, std::vector<std::string>& args);
    void parse();
};
