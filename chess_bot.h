#ifndef CHESS_BOT_H
#define CHESS_BOT_H

#include <string>
#include <forward_list>
#include <vector>
#include <memory>
#include <tuple>

class Evaluator {
public:
    static double evaluate(char squares[8][8]);
    static double evaluate_pawn(int row, int col, char squares[8][8], bool white);
    static double evaluate_knight(int row, int col, char squares[8][8]);
    static double evaluate_bishop(int row, int col, char squares[8][8]);
    static double evaluate_rook(int row, int col, char squares[8][8]);
    static double evaluate_queen(int row, int col, char squares[8][8], bool white);
    static double evaluate_king(int row, int col, char squares[8][8], bool white);
};

class MoveGenerator {
public:
    static std::forward_list<std::string> generate_all_moves(char squares[8][8], bool white_to_move, bool castle_rights[4], int en_passant_rights);
};

class Board {
public:
    Board(const char* init_squares, const std::string& previous_move, bool init_white_to_move, bool castle_rights[4], int en_passant_rights, int depth);
    std::tuple<char*, bool*, int> move(std::string current_move);
    void set_evaluation();
    double get_evaluation() const;
    std::string get_best_continuation() const;
    std::string get_previous_move() const;
    void print_continuations();
};

#endif