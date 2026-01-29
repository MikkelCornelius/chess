#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <tuple>

class Board {
    private:
    char squares[8][8];
    std::string previous_move;
    bool white_to_move;
    bool castle_rights[4];
    int en_passant_rights;

    public:
    Board(const char* init_squares, const std::string& previous_move, bool init_white_to_move, bool (&castle_rights)[4], int en_passant_rights);
    std::tuple<char*, bool*, int> move(std::string current_move);
    void set_evaluation();
    std::string get_previous_move() const {return previous_move;}
    char (&get_squares())[8][8] {return squares;}
    bool is_white_to_move() {return white_to_move;}
    bool (&get_castle_rights())[4] {return castle_rights;}
    int get_en_passant_rights() {return en_passant_rights;}
};

#endif