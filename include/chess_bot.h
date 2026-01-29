#ifndef CHESS_BOT_H
#define CHESS_BOT_H

#include "board.h"
#include <string>
#include <tuple>

std::tuple<double, std::string> mm_search(Board board, int depth);
extern "C" const char* get_move(const char* board_str);

#endif