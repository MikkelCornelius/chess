#include <tuple>
#include <string>
#include "../include/mover.h"
using namespace std;

std::tuple<char*, bool*, int> Mover::move(char (&squares)[8][8], bool (&castle_rights)[4], string current_move) {
    // Create copy of board
    char* new_board = new char[64];
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            new_board[i*8 + j] = squares[i][j];
    
    // Decode move on new_board using ASCII subtraction
    int from_col = current_move[0] - 'a';
    int from_row = '8' - current_move[1];
    int to_col = current_move[2] - 'a';
    int to_row = '8' - current_move[3];

    // Determine move type
    char type;
    if (squares[from_row][from_col] == 'P' && to_row == 0) {
        type = 'p'; //promotion
    } else if (squares[from_row][from_col] == 'p' && to_row == 7) {
        type = 'p'; //promotion
    } else if (tolower(static_cast<unsigned char>(squares[from_row][from_col])) == 'p' && from_col != to_col && squares[to_row][to_col] == ' ') {
        type = 'e'; //en passant
    } else if (tolower(static_cast<unsigned char>(squares[from_row][from_col])) == 'k' && abs(to_col - from_col) == 2) {
        type = 'c'; //castling
    } else {
        type = 'n'; //normal move
    }

    // Move
    new_board[to_row * 8 + to_col] = new_board[from_row * 8 + from_col];
    new_board[from_row * 8 + from_col] = ' ';

    // Handle speacial type move
    switch (type)
    {
    case 'n': //normal move
        break;
    case 'p': //promotion
        if (isupper(new_board[to_row * 8 + to_col])) {
            new_board[to_row * 8 + to_col] = 'Q'; //promote to queen
        } else {
            new_board[to_row * 8 + to_col] = 'q';
        }
        break;
    case 'e': //en passant
        if (isupper(new_board[to_row * 8 + to_col])) { //white
            new_board[(to_row + 1) * 8 + to_col] = ' '; //remove captured pawn
        } else { //black
            new_board[(to_row - 1) * 8 + to_col] = ' ';
        }
        break;
    case 'c': //castling
        if (to_col == 6) { //short castle
            new_board[to_row * 8 + 5] = new_board[to_row * 8 + 7]; //move rook
            new_board[to_row * 8 + 7] = ' ';
        } else {
            new_board[to_row * 8 + 3] = new_board[to_row * 8 + 0]; //long castle
            new_board[to_row * 8 + 0] = ' ';
        }
    }

    // Make castling rights
    bool* new_castle_rights = new bool[4];
    for (int i=0; i<4; i++) {new_castle_rights[i] = castle_rights[i];}
    if (new_board[to_row * 8 + to_col]=='K') { //remove castle rights for white when the king moves
        new_castle_rights[0] = false;
        new_castle_rights[1] = false;
    } else if (new_board[to_row * 8 + to_col]=='k') { //for black
        new_castle_rights[2] = false;
        new_castle_rights[3] = false;
    } else if (new_board[to_row * 8 + to_col]=='R') { //white rook
        if (from_col==7) {
            new_castle_rights[0] = false; //remove short castle rights when king side rook moves
        } else if (from_col==0) {
            new_castle_rights[1] = false; //or long castle
        }
    } else if (new_board[to_row * 8 + to_col]=='r') { //black rook
        if (from_col==7) {
            new_castle_rights[2] = false;
        } else if (from_col==0) {
            new_castle_rights[3] = false;
        }
    }

    // Make en passant rights
    int new_en_passant_rights;
    if ((new_board[to_row * 8 + to_col]=='P' && to_row-from_row==-2) || (new_board[to_row * 8 + to_col]=='p' && to_row-from_row==2)) { //if pawn double moved
        new_en_passant_rights = to_col;
    } else {
        new_en_passant_rights = 8;
    }

    return {new_board, new_castle_rights, new_en_passant_rights};
}