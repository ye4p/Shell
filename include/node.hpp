#pragma once

#include <vector>
#include <string>

enum class RedirectionType {
    Input,
    Output,
};

struct Word {
    std::string word;
};

struct Redirection {
    RedirectionType rt;
    Word arg;
}

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
    Error,
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

