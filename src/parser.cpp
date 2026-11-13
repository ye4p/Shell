#include "../include/parser.hpp"


//
//  Lexer function definitions
//

Lexer::Lexer() : line_(nullptr), quote_active_(false), quote_type_(' '), tt_(TokenType::Uninitialized), i(0) {
    
}

void Lexer::point_to(const std::string* line) {
    line_ = line;
}

std::tuple<ParseStatus, std::vector<Token>> Lexer::tokenize() {
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
                if (!quote_active_) {
                    push_token_if_nonempty();
                    quote_active_ = true;
                    quote_type_ = curr();
                    tt_ = TokenType::String;
                    tok_str_.clear();
                } else if (quote_active_ && curr() == quote_type_) {
                    quote_active_ = false;
                    quote_type_ = ' ';
                    push_token();
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
                push_token_if_nonempty();
                if (next() == '(') {
                    tt_ = TokenType::ProcessSubstitutionBegin;
                    tok_str_ += curr();
                    tok_str_ += next();
                    advance();
                } else {
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
                }
                break;
            case ')':
                push_token_if_nonempty();
                tok_str_ = curr();
                tt_ = TokenType::BackParentheses;
                push_token();
                break;
            case ' ':
                if (tt_ != TokenType::Whitespace) {
                    push_token_if_nonempty();
                    tt_ = TokenType::Whitespace;
                }
                tok_str_ += curr();
                break;
            case ';':
                push_token_if_nonempty();
                tt_ = TokenType::CommandSeparator;
                tok_str_ += curr();
                push_token();
                break;
            case '&':
                push_token_if_nonempty();
                // Can either be background operator or logical and
                if (!is_oob(i + 1) && next() == '&') {
                    tt_ = TokenType::LogicalOperator;
                    tok_str_ += curr();
                    tok_str_ += next();
                    advance();
                } else {
                    tt_ = TokenType::BackgroundOperator;
                    tok_str_ += curr();
                }
                push_token();
                break;
            default:
                if (tt_ == TokenType::Whitespace) 
                    push_token_if_nonempty();
                tok_str_ += curr();
                tt_ = TokenType::String;
        }

        advance();
    }
    if (!tok_str_.empty()) {
        push_token();
    }
    return {quote_active_ ? ParseStatus::KeepParsing : ParseStatus::Success, tokens_};
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

Parser::Parser(std::vector<std::string>& args) : tokens_ptr_(nullptr), args_(args), position_(0) {}

void Parser::point_to(const std::vector<Token>* tokens_ptr) {
    tokens_ptr_ = tokens_ptr;
}

CommandSeparator Parser::parse() {
    return parse_command_separator();
}

void Parser::cleanup() {

    for (size_t i = 0; i < tokens_ptr_->size(); ++i) {
        const Token& t = (*tokens_ptr_)[i];

        if (t.tt != TokenType::Whitespace) {
            tokens_.push_back(t);
        }
    }

    std::cout << "Filtered:\n";
    for (const Token& t : tokens_) {
        std::cout << t << '\n';
    }
}

CommandSeparator Parser::parse_command_separator() {
    std::vector<LogicalExpression> vec = {
        parse_logical_expression()
    };

    while (!is_oob() && curr().tt == TokenType::CommandSeparator) {
        advance();
        vec.emplace_back(
            parse_logical_expression()
        );
    }

    return CommandSeparator{vec};
}

LogicalExpression Parser::parse_logical_expression() {
    std::vector<Pipeline> vec = {
        parse_pipeline()
    };

    std::vector<LogicalOperator> ops;

    while (!is_oob() && curr().tt == TokenType::LogicalOperator) {
        ops.emplace_back(curr().content == "&&" ? LogicalOperator::And : LogicalOperator::Or);
        advance();
        vec.emplace_back(
            parse_pipeline()
        );
    }

    return LogicalExpression {
        .pipelines = vec,
        .ops = ops,
    };
}

Pipeline Parser::parse_pipeline() {
    std::vector<Command> vec = {
        parse_command()
    };

    std::vector<PipelineType> ops;

    while (!is_oob() && curr().tt == TokenType::Pipeline) {
        ops.push_back(PipelineType::Standard); // Combined not yet implemented
        advance();
        vec.push_back(parse_command());
    }

    return Pipeline {
        .commands = vec,
        .ops = ops,
    };
}

Command Parser::parse_command() {
    std::vector<Word> vec = {
        parse_word()
    };

    std::vector<Redirection> redirections;

    while (!is_oob() && (curr().tt == TokenType::InputRedirection || curr().tt == TokenType::OutputRedirection || curr().tt == TokenType::String)) {
        if (curr().tt == TokenType::String) {
            vec.emplace_back(
                parse_word()
            );
        } else {
            RedirectionType r = curr().tt == TokenType::InputRedirection ? RedirectionType::Input : RedirectionType::Output;
            advance();
            redirections.emplace_back(
                Redirection {
                    r,
                    parse_word(),
                }
            );
        }
    }

    while (!is_oob() && curr().tt == TokenType::String) {
        vec.emplace_back(
            parse_word()
        );
    }

    return Command {
        .args = vec,
        .redirections = redirections,
    };
}

Word Parser::parse_word() {
    const Token& t = curr();
    advance();

    // In theory it should only add TokenType::String
    return Word {
        t.content
    };
}

void Parser::advance() {
    ++position_;
}

const Token& Parser::curr() {
    return tokens_[position_];
}

bool Parser::is_oob() {
    return position_ >= tokens_.size();
}
