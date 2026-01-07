#include <iostream>
#include <ctime>
#include <string>
#include "chess_bot.h"

using namespace std;

// Compile with: g++ -std=c++20 -o bot_benchmark.out bot_benchmark.cpp chess_bot.cpp

/*const char* get_move(const char* board_str) {
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
    cout << "running with depth " << depth << endl;
    Board board(&squares[0][0], string(), white_to_move, castle_rights, en_passant_rights, depth);

    static string result_storage;
    result_storage = board.get_best_continuation();
    cout << "Evaluation: " << board.get_evaluation() << endl;
    return result_storage.c_str();
}*/

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