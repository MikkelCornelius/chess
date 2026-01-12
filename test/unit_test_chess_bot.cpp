#include <iostream>
#include <cassert>
#include "../include/chess_bot.h"

using namespace std;

// Compile with: g++ -std=c++20 -o test_chess_bot.exe test/unit_test_chess_bot.cpp src/chess_bot.cpp

void test_evaluator() {
    double tolerance = 0.0001; //use tolerance for floating point rounding errors

    // Test evaluate_pawn
    char board[8][8] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    };

    // Place a white pawn at e4 (row 4, col 4)
    board[4][4] = 'P';
    double score = Evaluator::evaluate_pawn(4, 4, board, true);
    assert(abs(score - (1.0 + 2.0)) < tolerance); // pawn value + position bonus for e4

    // Test knight evaluation
    board[4][4] = 'N';
    score = Evaluator::evaluate_knight(4, 4, board);
    assert(abs(score - (3.0 + 2.0)) < tolerance); // knight value + position bonus for e4

    // Test bishop evaluation
    board[4][4] = 'B';
    score = Evaluator::evaluate_bishop(4, 4, board);
    assert(abs(score - (3.0 + 1.0)) < tolerance); // bishop value + position bonus for e4

    // Test rook evaluation
    board[4][4] = 'R';
    score = Evaluator::evaluate_rook(4, 4, board);
    assert(abs(score - (4.0 + 0.2*14)) < tolerance); // rook value + control bonus for open file and rank

    // Test rook controlled squares
    board[4][5] = 'P';
    board[4][3] = 'P';
    board[5][4] = 'P';
    board[3][4] = 'P';
    score = Evaluator::evaluate_rook(4, 4, board);
    assert(abs(score - (4.0 + 0.2*0)) < tolerance); // rook value + no control bonus

    // Test queen evaluation
    board[4][4] = 'Q';
    score = Evaluator::evaluate_queen(4, 4, board, true);
    assert(abs(score - (9.0 + 0.5)) < tolerance); // queen value + position bonus for e4

    // Test king evaluation
    board[4][4] = 'K';
    score = Evaluator::evaluate_king(4, 4, board, true);
    assert(abs(score - (0.0 + (-4.0))) < tolerance); // king value + position bonus for e4

    // Test overall evaluation
    char board2[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    score = Evaluator::evaluate(board2); // Should be 0.0 for starting position
    assert(abs(score - 0.0) < tolerance);

    // Test gameover
    board2[0][4] = ' '; // remove black king
    score = Evaluator::evaluate(board2);
    assert(abs(score - 1000.0) < tolerance); // white wins
    board2[0][4] = 'k'; // restore black king
    board2[7][4] = ' '; // remove white king
    score = Evaluator::evaluate(board2);
    assert(abs(score - (-1000.0)) < tolerance); // black wins

    // Test random position
    char board3[8][8] = {
        {'r', ' ', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', ' ', 'p', 'p', 'p', 'p'},
        {' ', 'n', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'p', ' ', ' ', ' ', ' '},
        {' ', ' ', 'P', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'N', ' ', ' ', ' '},
        {'P', 'P', ' ', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', ' ', 'R'}
    };

    score = Evaluator::evaluate(board3);
    assert(abs(score - (-3.5)) < tolerance); // Expected score based on material and position

    cout << "Evaluator tests passed!" << endl;
}

void test_move_generator() {
    char board[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', ' ', 'p', 'p', 'p', 'p', 'p', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'p'},
        {'P', 'p', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'P', ' ', ' ', ' ', ' '},
        {' ', 'P', 'P', ' ', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', ' ', ' ', 'R'}
    };

    bool castle_rights[4] = {true, true, true, true};
    int en_passant = 1;

    auto moves = MoveGenerator::generate_all_moves(board, true, castle_rights, en_passant);

    // Should generate moves for white pieces
    assert(!moves.empty());

    // Check if a specific move is generated
    bool found_e2e4 = false; //pawn double move
    bool found_e2e3 = false; //pawn single move
    bool found_a1a2 = false; //rook move
    bool found_a1b1 = false; //fake move should be false
    bool found_b1c3 = false; //knight move
    bool found_c1e3 = false; //bishop move
    bool found_c1h6 = false; //capture
    bool found_d1d2 = false; //queen move
    bool found_e1f1 = false; //king move
    bool found_e1g1 = false; //castle move
    bool found_a5b6 = false; //pawn capture en passant

    for (const auto& move : moves) {
        if (move == "e2e4") {
            found_e2e4 = true;
        }
        if (move == "e2e3") {
            found_e2e3 = true;
        }
        if (move == "a1a2") {
            found_a1a2 = true;
        }
        if (move == "a1b1") {
            found_a1b1 = true;
        }
        if (move == "b1c3") {
            found_b1c3 = true;
        }
        if (move == "c1e3") {
            found_c1e3 = true;
        }
        if (move == "c1h6") {
            found_c1h6 = true;
        }
        if (move == "d1d2") {
            found_d1d2 = true;
        }
        if (move == "e1f1") {
            found_e1f1 = true;
        }
        if (move == "e1g1") {
            found_e1g1 = true;
        }
        if (move == "a5b6") {
            found_a5b6 = true;
        }
    }
    assert(found_e2e4);
    assert(found_e2e3);
    assert(found_a1a2);
    assert(!found_a1b1);
    assert(found_b1c3);
    assert(found_c1e3);
    assert(found_c1h6);
    assert(found_d1d2);
    assert(found_e1f1);
    assert(found_e1g1);
    assert(found_a5b6);

    cout << "Move generator tests passed!" << endl;
}

void test_bot() {
    // Test for checkmate
    cout << "Testing bot for checkmate scenario..." << endl;
    char squares[8][8] = {
        {' ', 'k', ' ', ' ', ' ', ' ', ' ', ' '},
        {'p', 'p', 'p', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'K', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    };
    bool castle_rights[4] = {false, false, false, false};
    int en_passant_rights = 8;

    Board board(&squares[0][0], "", true, castle_rights, en_passant_rights, 3);

    string move = board.get_best_continuation().substr(2,4);
    assert(move == "h3h8"); // Rook moves to h8 to deliver checkmate

    cout << "Checkmate test passed!" << endl;
}

int main() {
    test_evaluator();
    test_move_generator();
    test_bot();
    cout << "All tests passed!" << endl;
    return 0;
}