#include "../include/board.h"
#include "../include/moveGenerator.h"
#include "../include/evaluator.h"
#include <string>
#include <vector>
#include <memory>
#include <tuple>

using namespace std;

Board::Board(const char* init_squares, const string& previous_move, bool init_white_to_move, bool castle_rights[4], int en_passant_rights, int depth) {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            squares[i][j] = init_squares[i*8 + j];
    this->previous_move = previous_move;
    this->white_to_move = init_white_to_move;
    for (int i=0; i<4; i++) {this->castle_rights[i] = castle_rights[i];}
    this->en_passant_rights = en_passant_rights;

    if (depth == 0) {
        this->best_continuation = previous_move;
    }
    if (depth > 0) {
        this->continuations = MoveGenerator::generate_all_moves(squares, white_to_move, castle_rights, en_passant_rights);

        // Create array of pointers to boards
        vector<unique_ptr<Board>> boards;
        int num_continuations = static_cast<int>(distance(continuations.begin(), continuations.end()));
        boards.reserve(num_continuations); //set length of array

        // Create all boards and store pointers
        for (const string& current_move : continuations) {
            char* buf;
            bool* new_castle_rights;
            int new_en_passant_rights;
            tie(buf, new_castle_rights, new_en_passant_rights) = move(current_move);

            boards.emplace_back(make_unique<Board>(buf, current_move, !white_to_move, new_castle_rights, new_en_passant_rights, depth-1));
            delete [] buf;
            delete [] new_castle_rights;

            if (depth == 1) {
                boards.back()->set_evaluation(); //evaluate boards of depth 0
            }
        }

        // Find best continuation
        double board_eval;
        bool first_child = true;
        this->eval = (white_to_move) ? -800.0 : 800.0; //initialize eval to extreme value
        for (const auto& board : boards) {
            // Get evaluation of each child board
            board_eval = board->get_evaluation();

            // If forced mate, find fastest mate. eval==1000.0 mean no king. Eval 1000.0-n mean mate in n moves
            if (board_eval > 900.0) {
                if (continuations.front() != "a1a1") { //ignore dummy move
                    board_eval -= 1;
                }
            }
            if (board_eval < -900.0) {
                if (continuations.front() != "a1a1") { //ignore dummy move
                board_eval += 1;
                }
            }
            
            // Find min/max eval
            if ((board_eval > this->eval || first_child) && white_to_move) {//max for white
                this->eval = board_eval;
                this->best_continuation = previous_move+"->"+board->get_best_continuation();
            } else if ((board_eval < this->eval || first_child) && !white_to_move) {//min for black
                this->eval = board_eval;
                this->best_continuation = previous_move+"->"+board->get_best_continuation();
            }
            first_child = false;
        }
    }
}

std::tuple<char*, bool*, int> Board::move(string current_move) {
    // Create copy of board
    char* new_board = new char[64];
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            new_board[i*8 + j] = squares[i][j];
    
    // Decode move on new_board using ASCII subtraction
    int from_col = current_move[0] - 'a';
    int from_row = '8' - current_move[1];
    int to_col = current_move[2] - 'a';
    int to_row = '8' - current_move[3];

    // Determine move type
    char type;
    if (squares[from_row][from_col] == 'P' && to_row == 0) {
        type = 'p'; //promotion
    } else if (squares[from_row][from_col] == 'p' && to_row == 7) {
        type = 'p'; //promotion
    } else if (tolower(static_cast<unsigned char>(squares[from_row][from_col])) == 'p' && from_col != to_col && squares[to_row][to_col] == ' ') {
        type = 'e'; //en passant
    } else if (tolower(static_cast<unsigned char>(squares[from_row][from_col])) == 'k' && abs(to_col - from_col) == 2) {
        type = 'c'; //castling
    } else {
        type = 'n'; //normal move
    }

    // Move
    new_board[to_row * 8 + to_col] = new_board[from_row * 8 + from_col];
    new_board[from_row * 8 + from_col] = ' ';

    // Handle speacial type move
    switch (type)
    {
    case 'n': //normal move
        break;
    case 'p': //promotion
        if (isupper(new_board[to_row * 8 + to_col])) {
            new_board[to_row * 8 + to_col] = 'Q'; //promote to queen
        } else {
            new_board[to_row * 8 + to_col] = 'q';
        }
        break;
    case 'e': //en passant
        if (isupper(new_board[to_row * 8 + to_col])) { //white
            new_board[(to_row + 1) * 8 + to_col] = ' '; //remove captured pawn
        } else { //black
            new_board[(to_row - 1) * 8 + to_col] = ' ';
        }
        break;
    case 'c': //castling
        if (to_col == 6) { //short castle
            new_board[to_row * 8 + 5] = new_board[to_row * 8 + 7]; //move rook
            new_board[to_row * 8 + 7] = ' ';
        } else {
            new_board[to_row * 8 + 3] = new_board[to_row * 8 + 0]; //long castle
            new_board[to_row * 8 + 0] = ' ';
        }
    }

    // Make castling rights
    bool* new_castle_rights = new bool[4];
    for (int i=0; i<4; i++) {new_castle_rights[i] = castle_rights[i];}
    if (new_board[to_row * 8 + to_col]=='K') { //remove castle rights for white when the king moves
        new_castle_rights[0] = false;
        new_castle_rights[1] = false;
    } else if (new_board[to_row * 8 + to_col]=='k') { //for black
        new_castle_rights[2] = false;
        new_castle_rights[3] = false;
    } else if (new_board[to_row * 8 + to_col]=='R') { //white rook
        if (from_col==7) {
            new_castle_rights[0] = false; //remove short castle rights when king side rook moves
        } else if (from_col==0) {
            new_castle_rights[1] = false; //or long castle
        }
    } else if (new_board[to_row * 8 + to_col]=='r') { //black rook
        if (from_col==7) {
            new_castle_rights[2] = false;
        } else if (from_col==0) {
            new_castle_rights[3] = false;
        }
    }

    // Make en passant rights
    int new_en_passant_rights;
    if ((new_board[to_row * 8 + to_col]=='P' && to_row-from_row==-2) || (new_board[to_row * 8 + to_col]=='p' && to_row-from_row==2)) { //if pawn double moved
        new_en_passant_rights = to_col;
    } else {
        new_en_passant_rights = 8;
    }

    return {new_board, new_castle_rights, new_en_passant_rights};
}

double Board::get_evaluation() const {
    return eval;
}

void Board::set_evaluation() {
    this->eval = Evaluator::evaluate(squares);
}

string Board::get_best_continuation() const {
    return best_continuation;
}

string Board::get_previous_move() const {
    return previous_move;
}
