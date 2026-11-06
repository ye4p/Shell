#include "../include/parser.h"

Parser::Parser(Line* line, std::vector<std::string>& args) : line_(line), args_(args) {
    quotes_count_ = 0;
    quote_type_ = ' ';
    quote_active_ = false;

    has_char_ = false;
    is_comment_ = false;
    ps = ParseStatus::Inactive;
}

void Parser::parse_line() {
    size_t beg = 0;
    size_t i = 0;

    while (i < line_->str.size()) {
        if (line_[i] == '\'' || line_[i] == '\"') {
            if (quotes_count_ % 2 == 0) { // Quotes beginning
                bool need_new_beginning = i < 1 || line_[i - 1] == ' '; // If the char before before isn't OOB and isn't space, then -> still part of the same token
                if (i != beg && need_new_beginning) { // Push whatever there was before if non zero length
                    push_token();
                }
                quote_active_ = true;
                quote_type_ = line_[i];
                ++quotes_count_;
                ++i;
                if (need_new_beginning) 
                    beg = i;
                ps = ParseStatus::KeepParsing;
                continue;
            }
            if (line_[i] == quote_type_) { // Quotes end
                ++quotes_count_;
                quote_active_ = false;
                quote_type_ = ' ';
                push_token();
                beg = i + 1;
                ++i;
                ps = ParseStatus::Success;
                continue;
            }
        }

        if (!quote_active_ && line_[i] != ' ') {
            has_char_ = true;
            token_.push_back(line_[i]);
            if (i >= 1 && line_[i - 1] == ' ') { // Now it sets the beginning when previous char was space and current one isn't
                if (line_[i] == '#') { // Beginning of comment
                    is_comment_ = true;
                    break;
                }
                token_ = line_[i];
            }
            ++i;
            continue;
        }

        if (!quote_active_ && has_char_ && beg != i && !is_quote(i)) {
            push_token();
            has_char_ = false;
            beg = i + 1;
            ++i;
            continue;
        }
        if (quote_active_)
            token_.push_back(line_[i]);
        ++i;
    }
    if (line_.back() != ' ' && !is_quote(i) && !is_comment_ && ps != ParseStatus::KeepParsing)
        push_token();
}

void Parser::parse() {
    parse_line();
    while (ps == ParseStatus::KeepParsing) { // Line doesn't get updated.
        line_->read_line();
        token_.push_back('\n');
        parse_line();
    }
}

void Parser::push_token() {
    args_.push_back(token_);
    token_.clear();
    has_char_ = false;
}

bool Parser::is_quote(size_t index) {
    return line_->str[index] == '\'' || line_->str[index] == '\"';
}
