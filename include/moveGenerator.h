#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <forward_list>
#include <string>

class MoveGenerator {
public:
    static std::forward_list<std::string> generate_all_moves(char (&squares)[8][8], bool white_to_move, bool (&castle_rights)[4], int en_passant_rights);
};

#endif