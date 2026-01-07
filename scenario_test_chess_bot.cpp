#include <iostream>
#include "chess_bot.h"

using namespace std;

// Compile with: g++ -std=c++20 -o test_chess_bot.out scenario_test_chess_bot.cpp chess_bot.cpp

int main() {

    char squares[8][8] = {
        {'r', 'n', 'b', 'q', 'r', ' ', 'k', ' '},
        {'p', 'p', ' ', 'p', 'B', 'p', 'p', 'p'},
        {' ', ' ', 'p', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'p', ' ', ' ', ' '},
        {'Q', ' ', 'P', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', 'P', ' ', ' ', ' ', 'P', ' '},
        {'P', ' ', ' ', 'P', 'P', 'P', 'B', 'P'},
        {'R', ' ', ' ', ' ', 'K', ' ', 'N', 'R'}
    };
    bool castle_rights[4] = {true, true, false, false};
    int en_passant_rights = 8;
    bool white_to_move = false;

    Board board(&squares[0][0], "", white_to_move, castle_rights, en_passant_rights, 1);
    string move = board.get_best_continuation();
    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: d8e7" << endl;

    return 0;
}