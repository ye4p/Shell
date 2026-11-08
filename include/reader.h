#pragma once

#include <iostream>
#include "parser.h"

class Reader {
    std::vector<std::string>& args_;
    
public:
    Reader(std::vector<std::string>& args);
    void read();
};
