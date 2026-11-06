#pragma once

#include <iostream>
#include <string>
#include <vector>

enum class ParseStatus {
    Success,
    Fail,
    KeepParsing,
};

class Parser { // The tokens for this parser are just strings.
    std::string& line_;
    std::vector<std::string>& args_;

    int quotes_count_;
    char quote_type_;
    bool quote_active_;

    bool has_char_;
    std::string token_; // Now using token system to overstep quotes edge cases

    std::string carry_; // From previous line if multiple
public:
    Parser(std::string& line, std::vector<std::string>& args);
    ParseStatus parse_line();
    void parse();
};
