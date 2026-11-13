#pragma once

#include <iostream>
#include <vector>
#include <string>


struct Word {
    std::string word;
};

enum class RedirectionType {
    Input,
    Output,
};

struct Redirection {
    RedirectionType type;
    Word word;
};

enum class SubstitutionType {
    Input,
    Output,
};

struct ProcessSubstitution {
    SubstitutionType st;
    std::vector<Word> args;
};

struct Command {
    std::vector<Word> args;
    std::vector<Redirection> redirections;
    std::vector<ProcessSubstitution> ps;
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
std::ostream& operator<<(std::ostream& out, const ProcessSubstitution& ps);
std::ostream& operator<<(std::ostream& out, const Redirection& r);
std::ostream& operator<<(std::ostream& out, const Word& w);

