#include "../include/board.h"
#include <string>

Board::Board(const char* init_squares, const std::string& previous_move, bool init_white_to_move, bool (&castle_rights)[4], int en_passant_rights) {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            squares[i][j] = init_squares[i*8 + j];
    this->previous_move = previous_move;
    this->white_to_move = init_white_to_move;
    for (int i=0; i<4; i++) {this->castle_rights[i] = castle_rights[i];}
    this->en_passant_rights = en_passant_rights;
}
