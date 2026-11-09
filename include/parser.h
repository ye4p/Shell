#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "line.h"

enum class ParseStatus {
    Inactive,
    Success,
    Fail,
    KeepParsing,
};

enum class TokenType {
    Uninitialized, // Just for debugging purposes

    Pipeline,       // |
    Redirection,    // > and other kinds

    Arg,
    SingleQuotedString,
    DoublyQuotedString,
    ProcessSubstitutionBegin,
    ProcessSubstitutionEnd,
    InputRedirection,
    Command,
    LogicalOperator,
};

// Contains basic information about token.
struct Token {
    std::string content_;
    TokenType tt_;
    Token(std::string content, TokenType tt) : content_(content), tt_(tt) {}
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
    Lexer(const std::string* line, int num);
    void point_to(const std::string* line);
    std::tuple<std::vector<Token>, ParseStatus> tokenize();
    char curr();
    char next();
    char at(size_t index);
    size_t size();
    bool is_oob(size_t index);
    void advance();
    void push_token();
};

// Builds the AST tree and parses the tokens.
class Parser {
    const std::vector<Token>& tokens_;
    std::vector<std::string>& args_;

public:
    Parser(std::vector<Token>& tokens, std::vector<std::string>& args);
    void parse();
};
