#ifndef MOVER_H
#define MOVER_H

#include <tuple>
#include <string>

class Mover {
public:
    static std::tuple<char*, bool*, int> move(char (&squares)[8][8], bool (&castle_rights)[4], std::string current_move);
};

#endif