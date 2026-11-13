#include "../include/node.hpp"

std::ostream& operator<<(std::ostream& out, const CommandSeparator& cs) {
    out << "Command Separator with following arguments: \n";

    for (size_t i = 0; i < cs.vec.size(); ++i) {
        out << cs.vec[i];
        if ((i + 1) < cs.vec.size()) 
            out << ", ";
    }

    out << '\n';

    return out;
}

std::ostream& operator<<(std::ostream& out, const LogicalExpression& le) {
    // le.pipelines.size() == le.ops.size() + 1 condition needs to be kept
    out << "Logical expression with following arguments: \n";
    for (size_t i = 0; i < le.pipelines.size(); ++i) {
        out << le.pipelines[i];
        if (i < le.ops.size())
            out << " with logical operator=" << (le.ops[i] == LogicalOperator::And ? "&&" : "||") << ", ";
    }

    out << '\n';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Pipeline& p) {
    out << "Pipeline with following arguments: \n";
    for (size_t i = 0; i < p.commands.size(); ++i) {
        out << p.commands[i];
        if (i < p.ops.size())
            out << " with pipeline type=" << (p.ops[i] == PipelineType::Standard ? "|" : "|&") << ", ";
    }

    out << '\n';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Command& c) {
    out << "Command with following redirections: \n";

    for (size_t i = 0; i < c.redirections.size(); ++i) {
        out << c.redirections[i];
    }

    out << "+ with the following arguments: \n";
    for (size_t i = 0; i < c.args.size(); ++i) {
        out << c.args[i] << ", ";
    }

    out << "+ with the following process substitutions: \n";
    out << c.ps;

    out << '\n';
    return out;
}

std::ostream& operator<<(std::ostream& out, const ProcessSubstitution& ps) {
    out << "Process substitution of type " << (ps.st == SubstitutionType::Input ? "INPUT" : "OUTPUT") << " with the following arguments: \n";

    for (size_t i = 0; i < ps.args.size(); ++i) {
        out << ps.args[i] << ", ";
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const Redirection& r) {
    out << (r.type == RedirectionType::Input ? "Input" : "Output") << " redirection of " << r.word << '\n';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Word& w) {
    out << w.word;

    return out;
}


