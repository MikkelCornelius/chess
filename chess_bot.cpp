#include <string>

extern "C" const char* get_move(const char* board_str) {
    Board board(board_str);
    bool white_to_move = board_str[64]=='w';

    return "e2e4";
}

struct Board {
    char squares[8][8];  // simple 8x8 board, use 'P', 'p', 'R', etc.

    Board(const char* board_str) {
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                squares[i][j] = board_str[i*8 + j];
    }

    double evaluate() {
        // Simple evaluation function
        return 0.0;
    }
};