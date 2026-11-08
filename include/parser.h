#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "line.h"

enum class ParseStatus {
    Inactive,
    Success,
    Fail,
    KeepParsing,
};

enum class TokenType {
    Pipeline,       // |
    Redirection,    // > and other kinds

    Arg,
    Quote,
    Command,
    LogicalOperator,
};

// Contains basic information about token.
class Token {
    TokenType tt_;
    std::string content_;
};

// Converts plain string into the list of tokens.
class Lexer {
    const std::string& line_;
    bool quote_active_;

public:
    Lexer(const std::string& line);
    std::vector<Token> tokenize();
};

// Builds the AST tree and parses the tokens.
class Parser {
    const std::vector<Token>& tokens_;
    std::vector<std::string>& args_;

public:
    Parser(std::vector<Token>& tokens, std::vector<std::string>& args);
    void parse();
};
