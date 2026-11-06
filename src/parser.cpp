#include "../include/parser.h"

Parser::Parser(std::string& line, std::vector<std::string>& args) : line_(line), args_(args) {
    quotes_count_ = 0;
    quote_type_ = ' ';
    quote_active_ = false;

    has_char_ = false;
}

ParseStatus Parser::parse_line() {
    size_t beg = 0;
    size_t i = 0;

    while (i < line_.size()) {
        if (line_[i] == '\'' || line_[i] == '\"') {
            if (quotes_count_ % 2 == 0) { // Quotes beginning
                bool need_new_beginning = i < 1 || line_[i - 1] == ' '; // If the char before before isn't OOB and isn't space, then -> still part of the same token
                if (i != beg && need_new_beginning) { // Push whatever there was before if non zero length
                    args_.push_back(line_.substr(beg, i - beg));
                }
                quote_active_ = true;
                quote_type_ = line_[i];
                ++quotes_count_;
                ++i;
                if (need_new_beginning) // NEW ISSUE: now it puts the whole string in including quotes which isn't what we want
                    beg = i;
                continue;
            }
            if (line_[i] == quote_type_) { // Quotes end
                ++quotes_count_;
                quote_active_ = false;
                quote_type_ = ' ';
                args_.push_back(line_.substr(beg, i - beg));
                has_char_ = false;
                beg = i + 1;
                ++i;
                continue;
            }
        }

        if (!quote_active_ && line_[i] != ' ') {
            has_char_ = true;
            if (i >= 1 && line_[i - 1] == ' ') { // Now it sets the beginning when previous char was space and current one isn't
                beg = i;
            }
            ++i;
            continue;
        }

        if (!quote_active_ && has_char_ && beg != i) {
            args_.push_back(line_.substr(beg, i - beg));
            has_char_ = false;
            beg = i + 1;
        }

        ++i;
    }
    if (line_.back() != ' ' && line_.back() != '\'' && line_.back() != '\"' )
        args_.push_back(line_.substr(beg));

    return quote_active_ ? ParseStatus::KeepParsing : ParseStatus::Success;
}

void Parser::parse() {
    parse_line();
}
