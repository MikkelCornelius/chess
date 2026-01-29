#include <string>
#include <tuple>
#include "../include/board.h"
#include "../include/moveGenerator.h"
#include "../include/evaluator.h"
#include "../include/mover.h"

using namespace std;

// compile with: g++ -std=c++20 -shared -fPIC -o chessbot.dll .\src\chess_bot.cpp .\src\board.cpp .\src\evaluator.cpp .\src\moveGenerator.cpp .\src\mover.cpp

tuple<double, string> mm_search(Board board, int depth) {
    double eval;
    string best_continuation;

    // Recursive function to perform minimax search, ensuring depth first allowing linear memory instead of exponential

    // Only evaluate leaf boards. Leaf boards, have no best continuation, so just return previous move
    if (depth == 0) {
        eval = Evaluator::evaluate(board.get_squares());
        best_continuation = board.get_previous_move();
        return {eval, best_continuation};
    } 
    else { // Non-leaf boards make new boards for all legal continuations

        // Generate legal continuations
        forward_list<string> continuations = MoveGenerator::generate_all_moves(board.get_squares(), 
                                                                                board.is_white_to_move(), 
                                                                                board.get_castle_rights(), 
                                                                                board.get_en_passant_rights());
        
        // For each continuation, create child board and call mm_search recursively
        double child_eval = (board.is_white_to_move()) ? -800.0 : 800.0; //initialize eval to extreme value
        string child_best_continuation;
        double child_eval_temp;
        string child_best_continuation_temp;
        for (const string& continuation : continuations) {

            // Move
            char* buf;
            bool new_castle_rights[4];
            bool* new_castle_rights_buf;
            int new_en_passant_rights;
            tie(buf, new_castle_rights_buf, new_en_passant_rights) = Mover::move(board.get_squares(), board.get_castle_rights(), continuation);
            for (int i=0; i<4; i++) {new_castle_rights[i] = new_castle_rights_buf[i];}

            // Create child board
            Board child_board(buf, continuation, !board.is_white_to_move(), new_castle_rights, new_en_passant_rights);
            delete [] buf;
            delete [] new_castle_rights_buf;

            // Recursive call
            tie(child_eval_temp, child_best_continuation_temp) = mm_search(child_board, depth-1);


            /// Update best eval and continuation ///

            // If forced mate, find fastest mate. eval==1000.0 mean no king. Eval 1000.0-n mean mate in n moves
            if (child_eval_temp > 900.0) {
                if (continuations.front() != "a1a1") { //ignore dummy move
                    child_eval_temp -= 1;
                }
            } else if (child_eval_temp < -900.0) {
                if (continuations.front() != "a1a1") { //ignore dummy move
                child_eval_temp += 1;
                }
            }

            // Update best eval and continuation based on minimax
            if (board.is_white_to_move()) { //maximizing player
                if (child_eval_temp > child_eval || continuation == continuations.front()) {
                    child_eval = child_eval_temp;
                    child_best_continuation = child_best_continuation_temp;
                }
            } else { //minimizing player
                if (child_eval_temp < child_eval || continuation == continuations.front()) {
                    child_eval = child_eval_temp;
                    child_best_continuation = child_best_continuation_temp;
                }
            }
        }

        eval = child_eval;
        best_continuation = board.get_previous_move() + "->" + child_best_continuation;
        return {eval, best_continuation};
    }
}

extern "C" const char* get_move(const char* board_str) {
    if (!board_str) return nullptr;

    // Format input
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
    int en_passant_rights = en_passant_rights_char - '0'; //ASCII subtraction

    // Run bot
    static string result_storage;
    double eval;
    int depth = 4;
    Board board(&squares[0][0], string(), white_to_move, castle_rights, en_passant_rights);
    tie(eval, result_storage) = mm_search(board, depth);

    // Format output
    string formatted_eval = (eval<0) ? "-"+to_string(abs(eval)) : "+"+to_string(abs(eval));
    result_storage = result_storage.substr(2) + formatted_eval;
    return result_storage.c_str();
}
