#include <string>
#include "../include/board.h"

using namespace std;

// compile with: g++ -std=c++20 -shared -fPIC -o chessbot.dll .\src\chess_bot.cpp .\src\board.cpp .\src\evaluator.cpp .\src\moveGenerator.cpp
// for tests: g++ -std=c++20 -o chessbot.out chess_bot.cpp

extern "C" const char* get_move(const char* board_str) {
    if (!board_str) return nullptr;

    char squares[8][8];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            squares[i][j] = board_str[i*8 + j];
        }
    }
    bool white_to_move = (board_str[64] == 'w');
    bool castle_rights[4];
    for (int i=65; i<69; i++) {
        castle_rights[i-65] = board_str[i]=='T';
    }
    char en_passant_rights_char = board_str[69];
    int en_passant_rights = en_passant_rights_char - '0';

    int depth = 4;
    Board board(&squares[0][0], string(), white_to_move, castle_rights, en_passant_rights, depth);

    static string result_storage;
    double eval = board.get_evaluation();
    string formatted_eval = (eval<0) ? "-"+to_string(abs(eval)) : "+"+to_string(abs(eval));
    result_storage = board.get_best_continuation().substr(2) + formatted_eval;
    return result_storage.c_str();
}
