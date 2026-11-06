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

class Parser { // The tokens for this parser are just strings.
    Line* line_;
    std::vector<std::string>& args_;

    int quotes_count_;
    char quote_type_;
    bool quote_active_;

    ParseStatus ps;
    bool has_char_;
    bool is_comment_;
    std::string token_; // Now using token system to overstep quotes edge cases

    std::string carry_; // From previous line if multiple
public:
    Parser(Line* line, std::vector<std::string>& args);
    void parse_line();
    void parse();
    void push_token();
    bool is_quote(size_t index);
};
