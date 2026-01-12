#include "../include/evaluator.h"

double Evaluator::evaluate(char squares[8][8]) {
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

double Evaluator::evaluate_pawn(int row, int col, char squares[8][8], bool white) {
    double score = 1.0;
    if (white) {
        score += pawn_eval[row][col];
    } else {
        score += pawn_eval[7-row][col];
    }

    return score;
    //add pawn chains, passed pawns(plus rook behind), doubled pawns, isolated pawns.
}

double Evaluator::evaluate_knight(int row, int col, char squares[8][8]) {
    double score = 3.0;
    score += knight_eval[row][col];
    return score;
    //potentially add bonus for protecting other minor pieces
}

double Evaluator::evaluate_bishop(int row, int col, char squares[8][8]) {
    double score = 3.0;
    score += bishop_eval[row][col];
    return score;
    //potentially add bonus for controlling long diagonals
}

double Evaluator::evaluate_rook(int row, int col, char squares[8][8]) {
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

double Evaluator::evaluate_queen(int row, int col, char squares[8][8], bool white) {
    double score = 9.0;
    if (white) {
        score += queen_eval[row][col];
    } else {
        score += queen_eval[7-row][col];
    }
    return score;
}

double Evaluator::evaluate_king(int row, int col, char squares[8][8], bool white) {
    double score = 0.0;
    if (white) {
        score += king_eval[row][col];
    } else {
        score += king_eval[7-row][col];
    }
    return score;
    //potentially add bonus for castling, safety evaluation. Add bonus near pawns
}