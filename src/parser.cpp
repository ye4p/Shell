#include "../include/parser.h"

Parser::Parser(std::string& line, std::vector<std::string>& args) : line_(line), args_(args) {
    quotes_count = 0;
    quote_type = ' ';
    quote_active = false;
}

ParseStatus Parser::parse() {
    size_t beg = 0;
    size_t i = 0;

    while (i < line_.size()) {
        if (line_[i] == '\'' || line_[i] == '\"') {
            if (quotes_count % 2 == 0) { // Quotes beginning
                quote_active = true;
                quote_type = line_[i];
                ++quotes_count;
                ++i;
                beg = i;
                continue;
            }
            if (line_[i] == quote_type) { // Quotes end
                ++quotes_count;
                quote_active = false;
                quote_type = ' ';
                args_.push_back(line_.substr(beg, i - beg));
                beg = i + 1;
                ++i;
                continue;
            }
        }

        if (line_[i] != ' ' || quote_active) {
            if (i >= 1 && line_[i - 1] == ' ') { // Now it sets the beginning when previos char was space and current one isn't
                beg = i;
            }
            ++i; // Next thing need to add is when there are big gaps between tokens, parser shouldn't treat them as tokens
            continue;
        }

        if (beg != i) { // Right now it assumes that right after token ended, new one begins,
                        // But this isn't always true since there can be multiple spaces between them.
            args_.push_back(line_.substr(beg, i - beg));
            beg = i + 1;
        }

        ++i;
    }
    if (line_.back() != ' ' || (line_.back() != ' ' && line_.back() == quote_type))
        args_.push_back(line_.substr(beg));

    return ParseStatus::Success;
}
