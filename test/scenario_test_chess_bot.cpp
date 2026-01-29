#include <iostream>
#include <string>
#include <cassert>
#include "../include/board.h"
#include "../include/chess_bot.h"

using namespace std;

// Compile with: g++ -std=c++20 -o test_chess_bot.exe .\test\scenario_test_chess_bot.cpp .\src\chess_bot.cpp .\src\board.cpp .\src\evaluator.cpp .\src\moveGenerator.cpp .\src\mover.cpp

// Thoughts: Castling is very valuable. As bot cannot see many moves ahead, it sometimes wrongly evaluates castling above capturing free materiale. This also applies to general positioning of pieces.

int main() {
    int depth = 4;
    static string result_storage;
    double eval;
    string move;

    cout << "For board 1" << endl;

    char squares1[8][8] = {
        {'r', 'n', 'b', 'q', 'r', ' ', 'k', ' '},
        {'p', 'p', ' ', 'p', 'B', 'p', 'p', 'p'},
        {' ', ' ', 'p', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'p', ' ', ' ', ' '},
        {'Q', ' ', 'P', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', 'P', ' ', ' ', ' ', 'P', ' '},
        {'P', ' ', ' ', 'P', 'P', 'P', 'B', 'P'},
        {'R', ' ', ' ', ' ', 'K', ' ', 'N', 'R'}
    };
    bool castle_rights[4] = {true, true, false, false};
    int en_passant_rights = 8;
    bool white_to_move = false;

    Board board1(&squares1[0][0], "", white_to_move, castle_rights, en_passant_rights);
    tie(eval, result_storage) = mm_search(board1, depth);
    move = result_storage.substr(2, 4);

    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: d8e7" << endl;
    cout << "Mikkel play'd move: e8e7" << endl << endl;
    cout << "----------------------------------" << endl << endl;
    assert(move == "d8e7"); // Check whether bot response changed from last version

    //

    cout << "For board 2" << endl;
    char squares2[8][8] = {
        {'r', ' ', ' ', 'q', 'k', ' ', 'n', 'r'},
        {'p', 'p', 'p', ' ', ' ', 'p', 'p', 'p'},
        {' ', ' ', 'n', 'p', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'p', ' ', ' ', ' '},
        {' ', ' ', 'P', ' ', ' ', ' ', ' ', ' '},
        {' ', 'Q', 'P', ' ', ' ', ' ', 'P', 'N'},
        {'P', 'P', ' ', ' ', 'P', 'P', 'B', 'P'},
        {'R', ' ', 'B', ' ', 'K', ' ', ' ', 'R'}
    };
    bool castle_rights2[4] = {true, true, true, true};
    int en_passant_rights2 = 8;
    bool white_to_move2 = false;

    Board board2(&squares2[0][0], "", white_to_move2, castle_rights2, en_passant_rights2);
    tie(eval, result_storage) = mm_search(board2, depth);
    move = result_storage.substr(2, 4);
    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: e6c8" << endl;
    cout << "Mikkel play'd move: d8c8" << endl << endl;
    cout << "----------------------------------" << endl << endl;

    // This test is currently failing. Bot suggests g8f6->g2c6->b7c6->h3f4, missing the dangerous white queen move g8f6->b3b7, prefering knight development instead
    assert(move == "g8f6"); // Check whether bot response changed from last version

    //

    cout << "For board 3" << endl;
    char squares3[8][8] = {
        {'r', ' ', 'q', ' ', 'k', ' ', ' ', 'r'},
        {'p', 'p', 'p', ' ', 'n', 'p', 'p', 'p'},
        {' ', ' ', 'n', 'p', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'P', ' ', ' ', ' '},
        {' ', ' ', 'P', ' ', ' ', ' ', ' ', ' '},
        {' ', 'Q', 'P', ' ', ' ', ' ', 'P', 'N'},
        {'P', 'P', ' ', ' ', 'P', ' ', 'B', 'P'},
        {'R', ' ', 'B', ' ', 'K', ' ', ' ', 'R'}
    };
    bool castle_rights3[4] = {true, true, true, true};
    int en_passant_rights3 = 8;
    bool white_to_move3 = false;

    Board board3(&squares3[0][0], "", white_to_move3, castle_rights3, en_passant_rights3);
    tie(eval, result_storage) = mm_search(board3, depth);
    move = result_storage.substr(2, 4);
    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: e6h3" << endl;
    cout << "Mikkel play'd move: e6h3" << endl << endl;
    cout << "----------------------------------" << endl << endl;

    // This test is currently failing. Bot suggests d6e5->e2e4->e7f5->h3f4, missing a free piece with e6h3->g2h3->c8h3, prefering center pawns instead
    // What is going on? Trading whites knight and bishop for one bishop is evaluated by the bot as losing 0.1 evaluation as the two white pieces are poorly positioned. 
    // This should not happen. Also, ignoring the free piece allows for quick castling, which is evaluated to gaining 4.5 points. Way too much.
    assert(move == "d6e5"); // Check whether bot response changed from last version

    //

    cout << "For board 4" << endl;
    char squares4[8][8] = {
        {'r', ' ', 'b', 'q', 'k', 'b', ' ', 'r'},
        {'p', 'p', 'p', ' ', 'p', ' ', ' ', ' '},
        {' ', ' ', 'n', ' ', ' ', ' ', ' ', 'p'},
        {' ', 'N', ' ', 'p', ' ', ' ', 'p', ' '},
        {' ', ' ', ' ', 'P', 'n', 'P', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', 'B', ' '},
        {'P', 'P', 'P', ' ', ' ', 'P', 'P', 'P'},
        {'R', ' ', ' ', 'Q', 'K', 'B', 'N', 'R'}
    };
    bool castle_rights4[4] = {true, true, true, true};
    int en_passant_rights4 = 8;
    bool white_to_move4 = true;

    Board board4(&squares4[0][0], "", white_to_move4, castle_rights4, en_passant_rights4);
    tie(eval, result_storage) = mm_search(board4, depth);
    move = result_storage.substr(2, 4);
    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: d1h5" << endl;
    cout << "Mikkel play'd move: d1h5" << endl << endl;
    cout << "----------------------------------" << endl << endl;

    assert(move == "g1f3"); // Check whether bot response changed from last version

    //

    cout << "For board 5" << endl;
    char squares5[8][8] = {
        {'r', ' ', 'b', ' ', 'q', ' ', ' ', ' '},
        {' ', 'p', 'p', 'k', ' ', ' ', 'b', ' '},
        {'p', ' ', 'n', 'p', 'r', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', 'Q', 'P', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', 'N', 'B', ' '},
        {'P', 'P', 'P', ' ', 'B', 'P', 'P', 'P'},
        {'R', ' ', ' ', ' ', 'K', ' ', ' ', 'R'}
    };
    bool castle_rights5[4] = {true, true, false, false};
    int en_passant_rights5 = 8;
    bool white_to_move5 = true;

    Board board5(&squares5[0][0], "", white_to_move5, castle_rights5, en_passant_rights5);
    tie(eval, result_storage) = mm_search(board5, depth);
    move = result_storage.substr(2, 4);
    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: c2c3" << endl;
    cout << "Mikkel recommends move: d4d5" << endl;
    cout << "Mikkel play'd move: a1a4" << endl << endl;
    cout << "----------------------------------" << endl << endl;

    // This test is currently failing. Bot suggests e1g1->e6e2->d4d5->g7b2, missing a free piece, prefering to castle instead
    assert(move == "e1g1"); // Check whether bot response changed from last version

    //

    cout << "For board 6" << endl;
    char squares6[8][8] = {
        {'r', ' ', 'b', 'k', 'q', ' ', ' ', ' '},
        {' ', 'p', 'p', ' ', ' ', ' ', 'b', ' '},
        {'p', ' ', ' ', ' ', 'r', ' ', ' ', ' '},
        {'n', ' ', ' ', 'p', 'N', ' ', ' ', ' '},
        {'P', ' ', ' ', 'P', ' ', ' ', 'B', ' '},
        {' ', ' ', ' ', 'Q', ' ', ' ', 'B', ' '},
        {' ', 'P', 'P', ' ', ' ', 'P', 'P', 'P'},
        {'R', ' ', ' ', ' ', 'K', ' ', ' ', 'R'}
    };
    bool castle_rights6[4] = {true, true, false, false};
    int en_passant_rights6 = 8;
    bool white_to_move6 = false;

    Board board6(&squares6[0][0], "", white_to_move6, castle_rights6, en_passant_rights6);
    tie(eval, result_storage) = mm_search(board6, depth);
    move = result_storage.substr(2, 4);
    cout << "^-^Bot recommends move: " << move << endl;
    cout << "Stockfish recommends move: e6h6" << endl;
    cout << "Brolsen play'd move: e6e5" << endl << endl;
    cout << "----------------------------------" << endl << endl;

    // This test is currently failing. Bot suggests a5c6->e1g1->c6e5->g4e6, unwisely sacrificing rook, prefering to remove knight from rim instead
    // Bot thinks white would prefer to castle rather than to take the rook.
    assert(move == "a5c4"); // Check whether bot response changed from last version
    return 0;
}