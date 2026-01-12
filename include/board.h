#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <forward_list>
#include <tuple>
using namespace std;

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
    Board(const char* init_squares, const std::string& previous_move, bool init_white_to_move, bool castle_rights[4], int en_passant_rights, int depth);
    std::tuple<char*, bool*, int> move(std::string current_move);
    void set_evaluation();
    double get_evaluation() const;
    std::string get_best_continuation() const;
    std::string get_previous_move() const;
    void print_continuations();
};

#endif