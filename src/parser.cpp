#include "../include/parser.h"

//
//  Lexer function definitions
//

Lexer::Lexer() : line_(nullptr), quote_active_(false), toke_str_(false), tt_(TokenType::Uninitialized) {
    
}

void Lexer::point_to(const std::string* line) {
    line_ = line;
}

std::vector<Token> Lexer::tokenize() {
    while (i < size()) {
        if (quote_active_ && curr() != quote_type_) {
            tok_str_ += curr();
            ++i;
            continue;
        }
        switch (curr()) {
            case '0': 
                if (quote_active_) {
                    tok_str_ += curr();
                } else {
                    continue;
                }
                break;
            case '|':
                if (!is_oob(i + 1) && next() != '|') {
                    // Push the existing token if not empty.
                    push_token();
                    tt_ = TokenType::Pipeline;
                    tok_str_ += curr();
                    if (next() == '&') {
                        tos_str_ += next();
                        ++i;
                    }
                    push_token();
                } else if (!is_oob(i + 1)) {
                    tt_ = TokenType::LogicalOperator;
                    tok_str_ += curr() + next();
                    ++i;
                    push_token();
                } {
                    // Handle oob
                }
                break;
            case '\'':
            case '\"':
                if (quote_active_ && quote_type_ == curr()) {
                    push_token();
                    quote_type_ = ' ';
                    quote_active_ = false;
                } else {
                    quote_active_ = true;
                    quote_type_ = curr();
                    if (curr() == '\'')
                        tt_ = TokenType::SingleQuotedString;
                    else 
                        tt_ = TokenType::DoublyQuotedString;
                }
                break;
            case '<': // Can be either input redirection < or process substitution <()
                push_token();
                if (next() == '(') {
                    tt_ = TokenType::ProcessSubstitutionBegin;
                    tok_str_ += curr() + next();

                    advance();
                    push_token();
                } else {
                    tok_str_ += curr();
                    tt_ = TokenType::InputRedirection;
                    push_token();
                }
        }

        advance();
    }
    if (!tok_str_.empty()) {
        push_token();
    }
}

char Lexer::prev() {
    return (*line_)[i - 1]
}

char Lexer::curr() {
    return (*line_)[i];
}

char Lexer::next() {
    return (*line_)[i + 1];
}

char Lexer::at(size_t index) {
    return (*line_([index]);
}

size_t Lexer::size() {
    return line_->size();
}

bool Lexer::is_oob(size_t index) {
    return index < 0 || index >= line_->size();
}

void Lexer::advance() {
    ++i;
}

void Lexer::push_token() {
    tokens_.push_back(Token(content_, tt_));
    tt_ = TokenType::Uninitialized;
    content_.clear();
}

//
//  Parser function definitions
//

Parser::Parser(const std::vector<Token>& tokens, std::vector<std::string>& args) : tokens_(tokens), args_(args) {}

void Parser::parse() {

}

