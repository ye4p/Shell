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

    int quotes_count;
    char quote_type;
    bool quote_active;
    std::string carry; // From previous line if multiple
public:
    Parser(std::string& line, std::vector<std::string>& args);
    ParseStatus parse();
};
