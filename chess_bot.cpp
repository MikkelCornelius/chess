#include <string>
#include <forward_list>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

// compile with: g++ -std=c++20 -shared -fPIC -o chessbot.dll chess_bot.cpp
// for tests: g++ -std=c++20 -o chessbot.out chess_bot.cpp

constexpr char col_indeces[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
constexpr char row_indeces[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

string white_pieces = "PNBRQK";
string black_pieces = "pnbrqk";
int knight_moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                            {1, -2}, {1, 2}, {2, -1}, {2, 1}};

class Evaluator {
    private:
    static constexpr double pawn_eval[8][8] = {
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
        {1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
        {0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
        {0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
        {0.5,-0.5,-1.0, 0.0, 0.0,-1.0,-0.5, 0.5},
        {0.5, 1.0, 1.0,-2.0,-2.0, 1.0, 1.0, 0.5},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    };
    static constexpr double knight_eval[8][8] = {
        {-5.0,-4.0,-3.0,-3.0,-3.0,-3.0,-4.0,-5.0},
        {-4.0,-2.0, 0.0, 0.0, 0.0, 0.0,-2.0,-4.0},
        {-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0,-3.0},
        {-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5,-3.0},
        {-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0,-3.0},
        {-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5,-3.0},
        {-4.0,-2.0, 0.0, 0.5, 0.5, 0.0,-2.0,-4.0},
        {-5.0,-4.0,-3.0,-3.0,-3.0,-3.0,-4.0,-5.0}
    };
    static constexpr double bishop_eval[8][8] = {
        {-2.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-2.0},
        {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,-1.0},
        {-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0,-1.0},
        {-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5,-1.0},
        {-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,-1.0},
        {-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,-1.0},
        {-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5,-1.0},
        {-2.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-2.0}
    };
    static constexpr double queen_eval[8][8] = {
        {-2.0,-1.0,-1.0,-0.5,-0.5,-1.0,-1.0,-2.0},
        {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,-1.0},
        {-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0,-1.0},
        {-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0,-0.5},
        { 0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0,-0.5},
        {-1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0,-1.0},
        {-1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,-1.0},
        {-2.0,-1.0,-1.0,-0.5,-0.5,-1.0,-1.0,-2.0}
    };
    static constexpr double king_eval[8][8] = {
        {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
        {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
        {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
        {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
        {-2.0,-3.0,-3.0,-4.0,-4.0,-3.0,-3.0,-2.0},
        {-1.0,-2.0,-2.0,-2.0,-2.0,-2.0,-2.0,-1.0},
        { 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
        { 2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0}
    };

    public:
    static double evaluate(char squares[8][8]) {
        bool w_king_alive = false;
        bool b_king_alive = false;
        double score = 0.0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                char piece = squares[i][j];
                switch (piece) {
                    case 'P': score += evaluate_pawn(i, j, squares, true); break;
                    case 'N': score += evaluate_knight(i, j, squares); break;
                    case 'B': score += evaluate_bishop(i, j, squares); break;
                    case 'R': score += evaluate_rook(i, j, squares); break;
                    case 'Q': score += evaluate_queen(i, j, squares, true); break;
                    case 'K': score += evaluate_king(i, j, squares, true); w_king_alive = true; break;
                    case 'p': score -= evaluate_pawn(i, j, squares, false); break;
                    case 'n': score -= evaluate_knight(i, j, squares); break;
                    case 'b': score -= evaluate_bishop(i, j, squares); break;
                    case 'r': score -= evaluate_rook(i, j, squares); break;
                    case 'q': score -= evaluate_queen(i, j, squares, false); break;
                    case 'k': score -= evaluate_king(i, j, squares, false); b_king_alive = true; break;
                }
            }
        }

        if (!w_king_alive) {return -1000.0;}
        else if (!b_king_alive) {return 1000.0;}
        else {return score;}
    }

    static double evaluate_pawn(int row, int col, char squares[8][8], bool white) {
        double score = 1.0;
        if (white) {
            score += pawn_eval[row][col];
        } else {
            score += pawn_eval[7-row][col];
        }

        return score;
        //add pawn chains, passed pawns(plus rook behind), doubled pawns, isolated pawns.
    }

    static double evaluate_knight(int row, int col, char squares[8][8]) {
        double score = 3.0;
        score += knight_eval[row][col];
        return score;
        //potentially add bonus for protecting other minor pieces
    }

    static double evaluate_bishop(int row, int col, char squares[8][8]) {
        double score = 3.0;
        score += bishop_eval[row][col];
        return score;
        //potentially add bonus for controlling long diagonals
    }

    static double evaluate_rook(int row, int col, char squares[8][8]) {
        double score = 4.0;
        
        // Evaluate on number of controled sqaures
        int i = row-1;
        while (i >= 0 && squares[i][col] == ' ') {
            score += 0.2;
            --i;}
        i = row+1;
        while (i < 8 && squares[i][col] == ' ') {
            score += 0.2;
            ++i;}
        i = col-1;
        while (i >= 0 && squares[row][i] == ' ') {
            score += 0.2;
            --i;}
        i = col+1;
        while (i < 8 && squares[row][i] == ' ') {
            score += 0.2;
            ++i;}

        return score;
    }

    static double evaluate_queen(int row, int col, char squares[8][8], bool white) {
        double score = 9.0;
        if (white) {
            score += queen_eval[row][col];
        } else {
            score += queen_eval[7-row][col];
        }
        return score;
    }

    static double evaluate_king(int row, int col, char squares[8][8], bool white) {
        double score = 0.0;
        if (white) {
            score += king_eval[row][col];
        } else {
            score += king_eval[7-row][col];
        }
        return score;
        //potentially add bonus for castling, safety evaluation. Add bonus near pawns
    }
};

class MoveGenerator {
public:

    static forward_list<string> generate_all_moves(char squares[8][8], bool white_to_move, bool castle_rights[4], int en_passant_rights) {
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
};

class Board {
    private:
    char squares[8][8];
    string previous_move;
    string best_continuation;
    bool white_to_move;
    bool castle_rights[4];
    int en_passant_rights;
    double eval;
    forward_list<string> continuations;
    int num_continuations = 0;

    public:
    Board(const char* init_squares, const string& previous_move, bool init_white_to_move, bool castle_rights[4], int en_passant_rights, int depth) {
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                squares[i][j] = init_squares[i*8 + j];
        this->previous_move = previous_move;
        this->white_to_move = init_white_to_move;
        for (int i=0; i<4; i++) {this->castle_rights[i] = castle_rights[i];}
        this->en_passant_rights = en_passant_rights;

        if (depth == 0) {
            this->best_continuation = previous_move;
        }
        if (depth > 0) {
            this->continuations = MoveGenerator::generate_all_moves(squares, white_to_move, castle_rights, en_passant_rights);

            // Create array of pointers to boards
            vector<unique_ptr<Board>> boards;
            int num_continuations = static_cast<int>(distance(continuations.begin(), continuations.end()));
            boards.reserve(num_continuations); //set length of array

            // Create all boards and store pointers
            for (const string& current_move : continuations) {
                char* buf;
                bool* new_castle_rights;
                int new_en_passant_rights;
                tie(buf, new_castle_rights, new_en_passant_rights) = move(current_move);

                boards.emplace_back(make_unique<Board>(buf, current_move, !white_to_move, new_castle_rights, new_en_passant_rights, depth-1));
                delete [] buf;
                delete [] new_castle_rights;

                if (depth == 1) {
                    boards.back()->set_evaluation(); //evaluate boards of depth 0
                }
            }

            // Find best continuation
            double board_eval;
            bool first_child = true;
            this->eval = (white_to_move) ? -800.0 : 800.0; //initialize eval to extreme value
            for (const auto& board : boards) {
                // Get evaluation of each child board
                board_eval = board->get_evaluation();

                // If forced mate, find fastest mate. eval==1000.0 mean no king. Eval 1000.0-n mean mate in n moves
                if (board_eval > 900.0) {
                    if (continuations.front() != "a1a1") { //ignore dummy move
                        board_eval -= 1;
                    }
                }
                if (board_eval < -900.0) {
                    if (continuations.front() != "a1a1") { //ignore dummy move
                    board_eval += 1;
                    }
                }
                
                // Find min/max eval
                if ((board_eval > this->eval || first_child) && white_to_move) {//max for white
                    this->eval = board_eval;
                    this->best_continuation = previous_move+"->"+board->get_best_continuation();
                } else if ((board_eval < this->eval || first_child) && !white_to_move) {//min for black
                    this->eval = board_eval;
                    this->best_continuation = previous_move+"->"+board->get_best_continuation();
                }
                first_child = false;
            }
        }
    }

    std::tuple<char*, bool*, int> move(string current_move) {
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

    double get_evaluation() const {
        return eval;
    }

    void set_evaluation() {
        this->eval = Evaluator::evaluate(squares);
    }

    string get_best_continuation() const {
        return best_continuation;
    }

    string get_previous_move() const {
        return previous_move;
    }
};

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
