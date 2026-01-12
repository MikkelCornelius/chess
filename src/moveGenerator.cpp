#include <string>
#include <forward_list>
#include "../include/moveGenerator.h"
using namespace std;

constexpr char col_indeces[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
constexpr char row_indeces[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

string white_pieces = "PNBRQK";
string black_pieces = "pnbrqk";
int knight_moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                            {1, -2}, {1, 2}, {2, -1}, {2, 1}};

forward_list<string> MoveGenerator::generate_all_moves(char squares[8][8], bool white_to_move, bool castle_rights[4], int en_passant_rights) {
    forward_list<string> moves;
    bool white_king_alive = false;
    bool black_king_alive = false;
    if (white_to_move) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                char piece = squares[i][j];
                switch (piece)
                {
                case ' ':
                    break;
                
                case 'P':
                    if (squares[i-1][j]==' ') { //simple move forward
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i-1]});
                    }
                    if (i==6 && squares[i-2][j]==' ' && squares[i-1][j]==' ') { //double move
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i-2]});
                    }
                    // captures
                    if (j-1 >= 0) {
                        if (black_pieces.find(squares[i-1][j-1]) != string::npos || (en_passant_rights==j-1 && i==3)) {
                            moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j-1], row_indeces[i-1]});
                        }
                    }
                    if (j+1 < 8) {
                        if (black_pieces.find(squares[i-1][j+1]) != string::npos || (en_passant_rights==j+1 && i==3)) {
                            moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j+1], row_indeces[i-1]});
                        }
                    }
                    break;
                
                case 'N':
                    for (auto& move : knight_moves) {
                        int dest_i = i + move[0];
                        int dest_j = j + move[1];
                        //looping through all 8 knight moves
                        if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                            if (squares[dest_i][dest_j] == ' ' || black_pieces.find(squares[dest_i][dest_j]) != string::npos) { //add legal move if destination is empty or has opponent piece
                                moves.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                            }
                        }
                    }
                    break;

                case 'Q': {
                    // Queen moves like both Rook and Bishop
                    // Fall through to Rook case first
                }
                case 'R': {
                    int k = i-1;
                    while (k >= 0 && squares[k][j] == ' ') { //create legal move until blocked
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        --k;}
                    if (k >= 0 && black_pieces.find(squares[k][j]) != string::npos) { //if opponent piece, add one more move (and check whether still in bound)
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                    }
                    k = i+1;
                    while (k < 8 && squares[k][j] == ' ') { //do the same in other direction
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        ++k;}
                    if (k < 8 && black_pieces.find(squares[k][j]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                    }
                    k = j-1;
                    while (k >= 0 && squares[i][k] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        --k;}
                    if (k >= 0 && black_pieces.find(squares[i][k]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                    }
                    k = j+1;
                    while (k < 8 && squares[i][k] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        ++k;}
                    if (k < 8 && black_pieces.find(squares[i][k]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                    }
                    if (piece != 'Q') break;  // Create fall through for queen
                }
                case 'B': {
                    int ki, kj;
                    ki = i-1; kj = j-1;
                    while (ki >= 0 && kj >= 0 && squares[ki][kj] == ' ') { //create legal move until blocked
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        --ki; --kj;}
                    if (ki >= 0 && kj >= 0 && black_pieces.find(squares[ki][kj]) != string::npos) { //if opponent piece, add one more move (and check whether still in bound)
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    ki = i-1; kj = j+1;
                    while (ki >= 0 && kj < 8 && squares[ki][kj] == ' ') { //do the same in other direction
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        --ki; ++kj;}
                    if (ki >= 0 && kj < 8 && black_pieces.find(squares[ki][kj]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    ki = i+1; kj = j-1;
                    while (ki < 8 && kj >= 0 && squares[ki][kj] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        ++ki; --kj;}
                    if (ki < 8 && kj >= 0 && black_pieces.find(squares[ki][kj]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    ki = i+1; kj = j+1;
                    while (ki < 8 && kj < 8 && squares[ki][kj] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        ++ki; ++kj;}
                    if (ki < 8 && kj < 8 && black_pieces.find(squares[ki][kj]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    break;
                }

                case 'K':
                    white_king_alive = true;
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            if (di == 0 && dj == 0) continue;
                            int dest_i = i + di;
                            int dest_j = j + dj;
                            if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) { //check out of bounds
                                if (squares[dest_i][dest_j] == ' ' || black_pieces.find(squares[dest_i][dest_j]) != string::npos) { //check whether square is free to move to
                                    moves.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                                }
                            }
                        }
                    }

                    // Castling
                    if (castle_rights[0]) { //white short castle
                        if (squares[7][5]==' ' && squares[7][6]==' ') {
                            moves.push_front({col_indeces[4], row_indeces[7], col_indeces[6], row_indeces[7]});
                        }
                    }
                    if (castle_rights[1]) { //white long castle
                        if (squares[7][3]==' ' && squares[7][2]==' ') {
                            moves.push_front({col_indeces[4], row_indeces[7], col_indeces[2], row_indeces[7]});
                        }
                    }
                    break;
                case 'k':
                    black_king_alive = true;
                    break;
                }
            }
        }
    } else {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                char piece = squares[i][j];
                switch (piece)
                {
                case ' ':
                    break;
                
                case 'p':
                    // simple one-step forward
                    if (squares[i+1][j]==' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i+1]});
                    }
                    // double move
                    if (i==1 && squares[i+1][j]==' ' && squares[i+2][j]==' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i+2]});
                    }
                    // captures
                    if (j-1 >= 0) {
                        if (white_pieces.find(squares[i+1][j-1]) != string::npos || (en_passant_rights==j-1 && i==4)) {
                            moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j-1], row_indeces[i+1]});
                        }
                    }
                    if (j+1 < 8) {
                        if (white_pieces.find(squares[i+1][j+1]) != string::npos || (en_passant_rights==j+1 && i==4)) {
                            moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j+1], row_indeces[i+1]});
                        }
                    }
                    break;
                
                case 'n':
                    for (auto& move : knight_moves) {
                        int dest_i = i + move[0];
                        int dest_j = j + move[1];
                        if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                            if (squares[dest_i][dest_j] == ' ' || white_pieces.find(squares[dest_i][dest_j]) != string::npos) {
                                moves.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                            }
                        }
                    }
                    break;

                case 'q': {
                    // Queen moves like both Rook and Bishop — handle via fall-through style but keep safe bounds check
                }
                case 'r': {
                    int k = i-1;
                    while (k >= 0 && squares[k][j] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        --k;}
                    if (k >= 0 && white_pieces.find(squares[k][j]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                    }
                    k = i+1;
                    while (k < 8 && squares[k][j] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        ++k;}
                    if (k < 8 && white_pieces.find(squares[k][j]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                    }
                    k = j-1;
                    while (k >= 0 && squares[i][k] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        --k;}
                    if (k >= 0 && white_pieces.find(squares[i][k]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                    }
                    k = j+1;
                    while (k < 8 && squares[i][k] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        ++k;}
                    if (k < 8 && white_pieces.find(squares[i][k]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                    }
                    if (piece != 'q') break; // fall through for queen
                }
                case 'b': {
                    int ki, kj;
                    ki = i-1; kj = j-1;
                    while (ki >= 0 && kj >= 0 && squares[ki][kj] == ' ') { //create legal move until blocked
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        --ki; --kj;}
                    if (ki >= 0 && kj >= 0 && white_pieces.find(squares[ki][kj]) != string::npos) { //if opponent piece, add one more move (and check whether still in bound)
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    ki = i-1; kj = j+1;
                    while (ki >= 0 && kj < 8 && squares[ki][kj] == ' ') { //do the same in other direction
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        --ki; ++kj;}
                    if (ki >= 0 && kj < 8 && white_pieces.find(squares[ki][kj]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    ki = i+1; kj = j-1;
                    while (ki < 8 && kj >= 0 && squares[ki][kj] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        ++ki; --kj;}
                    if (kj >= 0 && ki < 8 && white_pieces.find(squares[ki][kj]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    ki = i+1; kj = j+1;
                    while (ki < 8 && kj < 8 && squares[ki][kj] == ' ') {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        ++ki; ++kj;}
                    if (ki < 8 && kj < 8 && white_pieces.find(squares[ki][kj]) != string::npos) {
                        moves.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                    }
                    break;
                }

                case 'k':
                    black_king_alive = true;
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            if (di == 0 && dj == 0) continue;
                            int dest_i = i + di;
                            int dest_j = j + dj;
                            if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                                if (squares[dest_i][dest_j] == ' ' || white_pieces.find(squares[dest_i][dest_j]) != string::npos) {
                                    moves.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                                }
                            }
                        }
                    }

                    // Castling
                    if (castle_rights[2]) { //black short castle
                        if (squares[0][5]==' ' && squares[0][6]==' ') {
                            moves.push_front({col_indeces[4], row_indeces[0], col_indeces[6], row_indeces[0]});
                        }
                    }
                    if (castle_rights[3]) { //black long castle
                        if (squares[0][3]==' ' && squares[0][2]==' ') {
                            moves.push_front({col_indeces[4], row_indeces[0], col_indeces[2], row_indeces[0]});
                        }
                    }
                    break;
                case 'K':
                    white_king_alive = true;
                    break;
                }
            }
        } 
    }
    if (!(white_king_alive && black_king_alive)) {
        moves.clear(); //can't play when a king is missing
        moves.push_front("a1a1"); //dummy move, does nothing
    }

    return moves;
}