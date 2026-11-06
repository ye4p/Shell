#pragma once

#include <string>
#include <iostream>

struct Line {
    std::string str;
    void read_line();
    operator[](size_t index) {

    }
};
