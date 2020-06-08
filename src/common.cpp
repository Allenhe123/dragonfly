#include "common.h"

bool file_exist(const std::string& file_name) {
    std::ifstream f(file_name.c_str());
    return f.good();
}