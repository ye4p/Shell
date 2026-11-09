#include "../include/parser.h"

//
//  Lexer function definitions
//

Lexer::Lexer() : line_(nullptr), quote_active_(false), quote_type_(' '), tt_(TokenType::Uninitialized), i(0) {
    
}

void Lexer::point_to(const std::string* line) {
    line_ = line;
}

std::tuple<std::vector<Token>, ParseStatus> Lexer::tokenize() {
    while (i < size()) {
        if (quote_active_ && curr() != quote_type_) {
            tok_str_ += curr();
            advance();
            continue;
        }
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
                if (quote_active_ && quote_type_ == curr()) {
                    push_token();
                    quote_type_ = ' ';
                    quote_active_ = false;
                } else {
                    push_token_if_nonempty();
                    quote_active_ = true;
                    quote_type_ = curr();
                    if (curr() == '\'')
                        tt_ = TokenType::SingleQuotedString;
                    else 
                        tt_ = TokenType::DoublyQuotedString;
                }
                break;
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

            case ')':
                push_token_if_nonempty();
                tok_str_ = curr();
                tt_ = TokenType::BackParentheses;
                push_token();
                break;
            case ' ':
                push_token_if_nonempty();
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
    return {tokens_, quote_active_ ? ParseStatus::KeepParsing : ParseStatus::Success};
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

