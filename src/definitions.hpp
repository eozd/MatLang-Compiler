#pragma once
#include <tuple>
#include <vector>
#include "token.hpp"

//stmt_with_info[0] --> token_list of the line
//stmt_with_info[1] --> statement category
//stmt_with_info[2] --> line number
typedef std::tuple<std::vector<Token>, TokenCategory, int> stmt_with_info;
