#pragma once

#include <iostream>
#include <vector>
#include <string>


struct Word {
    std::string word;
};

enum class Redirection {
    Input,
    Output,
};

struct Command {
    std::vector<Word> args;
    std::vector<Redirection> redirections;
    // Also need to add assignment support
};

enum class PipelineType {
    Standard,
//    Combined,
};

struct Pipeline {
    std::vector<Command> commands;
    std::vector<PipelineType>  ops;
};

enum class LogicalOperator {
    Error = -1,
    Null,
    And, 
    Or,
};

struct LogicalExpression {
    std::vector<Pipeline> pipelines;
    std::vector<LogicalOperator> ops;
};

struct CommandSeparator {   // Separated by ; or & (in the future)
    std::vector<LogicalExpression> vec;
};


std::ostream& operator<<(std::ostream& out, const CommandSeparator& cs);
std::ostream& operator<<(std::ostream& out, const LogicalExpression& le);
std::ostream& operator<<(std::ostream& out, const Pipeline& p);
std::ostream& operator<<(std::ostream& out, const Command& c);
std::ostream& operator<<(std::ostream& out, const Word& w);

