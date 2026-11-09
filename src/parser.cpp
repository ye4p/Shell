#include "../include/parser.h"

//
//  Lexer function definitions
//

Lexer::Lexer() : line_(nullptr), tt_(TokenType::Uninitialized), i(0) {
    
}

void Lexer::point_to(const std::string* line) {
    line_ = line;
}

std::vector<Token> Lexer::tokenize() {
    while (i < size()) {
        switch (curr()) {
            case '|':
                if (!is_oob(i + 1) && next() != '|') {
                    push_token_if_nonempty();
                    tt_ = TokenType::Pipeline;
                    tok_str_ += curr();
                    if (next() == '&') {
                        tok_str_ += next();
                        advance();
                    }
                    push_token();
                } else if (!is_oob(i + 1)) {
                    tt_ = TokenType::LogicalOperator;
                    tok_str_ += curr() + next();
                    advance();
                    push_token();
                } 
                break;
            case '\'':
            case '\"':
                push_token_if_nonempty();
                tok_str_ += curr();
                if (curr() == '\'')
                    tt_ = TokenType::SingleQuote;
                else 
                    tt_ = TokenType::DoubleQuote;
                push_token();

            case '<': // Can be either input redirection < or process substitution <()
                push_token_if_nonempty();
                if (!is_oob(i + 1) && next() == '(') {
                    tt_ = TokenType::ProcessSubstitutionBegin;
                    tok_str_ += curr();
                    tok_str_ += next();

                    advance();
                    push_token();
                } else {
                    tok_str_ += curr();
                    tt_ = TokenType::InputRedirection;
                    push_token();
                }
                break;
            case '>':
                push_token_if_nonempty();
                tt_ = TokenType::OutputRedirection;

                if (!is_oob(i - 1) && std::isdigit(prev()) && prev() == tokens_.back().content.front()) {
                    tok_str_ += prev();
                    tokens_.pop_back();
                }

                tok_str_ += curr();

                if (!is_oob(i + 1) && next() == '>') {  // LEGAL: smth > smth, smth >> smth, 
                                                        // smth >& smth, smth >>& smth
                                                        // and any combinations with numbers on both sides.
                    tok_str_ += next();
                    advance();
                }

                if (!is_oob(i + 1) && next() == '&') {
                    tok_str_ += next();
                    advance();
                }

                if (!is_oob(i + 1) && std::isdigit(next())) {
                    tok_str_ += next();
                    advance();
                }

                push_token();

                break;
            case ')':
                push_token_if_nonempty();
                tok_str_ = curr();
                tt_ = TokenType::BackParentheses;
                push_token();
                break;
            case ' ':
                if (tt_ != TokenType::Whitespace)
                    push_token_if_nonempty();
                tok_str_ += curr();
                break;
            default:
                tok_str_ += curr();
                tt_ = TokenType::String;
        }

        advance();
    }
    if (!tok_str_.empty()) {
        push_token();
    }
    return tokens_;
}

char Lexer::prev() {
    return (*line_)[i - 1];
}

char Lexer::curr() {
    return (*line_)[i];
}

char Lexer::next() {
    return (*line_)[i + 1];
}

char Lexer::at(size_t index) {
    return (*line_)[index];
}

size_t Lexer::size() {
    return line_->size();
}

bool Lexer::is_oob(size_t index) {
    return index >= line_->size();
}

void Lexer::advance() {
    ++i;
}

void Lexer::push_token() {
    tokens_.push_back(Token(tok_str_, tt_));
    tt_ = TokenType::Uninitialized;
    tok_str_.clear();
}

bool Lexer::push_token_if_nonempty() { // Returns true/false if non empty token was added
    if (tok_str_.empty()) return false;

    tokens_.push_back(Token(tok_str_, tt_));
    tt_ = TokenType::Uninitialized;
    tok_str_.clear();
    return true;
}

//
//  Parser function definitions
//

Parser::Parser(const std::vector<Token>& tokens, std::vector<std::string>& args) : tokens_(tokens), args_(args) {}

void Parser::parse() {

}

