#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "node.h"

enum class ParseStatus {
    Inactive,
    Success,
    Fail,
    KeepParsing,
};

enum class TokenType {
    Uninitialized = -1, // Just for debugging purposes

    Whitespace = 0,
    String = 1,
    SingleQuote = 2,
    DoubleQuote = 3,
    Pipeline = 4,       // |
    InputRedirection = 5,
    OutputRedirection = 6,    // > and other kinds
    ProcessSubstitutionBegin = 7, // Several things will have beginning like this, but the end will be just generic parenthese
    BackParentheses = 8,
    LogicalOperator = 9,
    CommandSeparator = 10,
    BackgroundOperator = 11,
};

// Contains basic information about token.
struct Token {
    std::string content;
    TokenType tt;
    Token(std::string content, TokenType tt) : content(content), tt(tt) {}
};

std::ostream& operator<<(std::ostream& out, const Token& t);

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
    std::vector<Token> tokenize();
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
    const std::vector<Token>* tokens_ptr_;
    std::vector<Token> tokens_;
    std::vector<std::string>& args_;

public:
    Parser(std::vector<std::string>& args);
    void point_to(const std::vector<Token>* tokens);
    // Ex: cat < input.txt | grep "hello world" > output.txt && echo success
    ParseStatus parse();
    void cleanup(); // Gets rid of extra stuff in the tokens vector
};
