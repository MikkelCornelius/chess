#include <iostream>
#include <ctime>
#include <string>
#include "../include/chess_bot.h"

using namespace std;

// Compile with: g++ -std=c++20 -o bot_benchmark.out bot_benchmark.cpp chess_bot.cpp

int main() {
    string start_pos =
        "rnbqkbnr"
        "pppppppp"
        "        "
        "        "
        "        " 
        "        "
        "PPPPPPPP"
        "RNBQKBNR"
        "w"
        "TTTT8";
    string italian_pos =
        "r bqkbnr"
        "pppp ppp"
        "  n     "
        "    p   "
        "  B P   " 
        "     N  "
        "PPPP PPP"
        "RNBQK  R"
        "b"
        "TTTT8";
    string midgame_pos =
        "rn    k "
        "p  prppp"
        "b p     "
        "    p   "
        "    QP  " 
        "  P   PN"
        "P  qP BP"
        "R    RK "
        "w"
        "FFFF8";
    string endgame_pos =
        "r       "
        "   R pkp"
        "      p "
        "        "
        "  p    P" 
        "Pp    P "
        " P   PK "
        "        "
        "w"
        "FFFF8";
    string en_pas_pos =
        "r bqkbnr"
        "pppn  pp"
        "   p p  "
        "   Pp   "
        "        " 
        "        "
        "PPP PPPP"
        "RNBQKBNR"
        "w"
        "TTTT4";


    clock_t start_time;
    clock_t end_time;
    for (string pos : {start_pos, italian_pos, midgame_pos, endgame_pos}) {
        start_time = clock();
        const char* res = get_move(pos.c_str());
        end_time = clock();
        double cpu_time_used = double(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
        cout << "For position:\n" << pos << "\nget_move returned: " << res << " after " << cpu_time_used << " ms\n" << endl;
    }

    return 0;
}