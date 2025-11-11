#include <string>
#include <forward_list>
#include <unordered_map>
#include <iostream>

// compile with: g++ -shared -fPIC -o chessbot.dll chess_bot.cpp

// TODO: evaluate board, find all legal continuations, determine depth, find best continuation

/*evaluate board:
so far simple material count is used. Should be improved later

legal continuations: (potential bug might be fixed: Bishop if statement breaks if out of bounds)
each continuation should create its own 'board'. A board should contain an array pointing to all boards that are direct continuations of said board, and each board should also contain a pointer to the board this board is a continuation from

determine depth:
How far we look, when searching for the best continuation. It can be a constant for starters, but should later be evaluated from the position

find best continuation:
When we have a tree, with some depth, of all continuations, we should evaluate the position on all leaf boards, from there go up until we reach the root*/

constexpr char col_indeces[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
constexpr char row_indeces[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

/*old implementation of col and row indeces
std::unordered_map<int, std::string> col_indeces = {
    {0, "a"}, {1, "b"}, {2, "c"}, {3, "d"},
    {4, "e"}, {5, "f"}, {6, "g"}, {7, "h"}
};

std::unordered_map<int, std::string> row_indeces = {
    {0, "8"}, {1, "7"}, {2, "6"}, {3, "5"},
    {4, "4"}, {5, "3"}, {6, "2"}, {7, "1"}
};*/

std::string white_pieces = "PNBRQK";
std::string black_pieces = "pnbrqk";
int knight_moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                            {1, -2}, {1, 2}, {2, -1}, {2, 1}};

/*extern "C" const char* get_move(const char* board_str) {
    Board board(board_str);

    return "e2e4";
}*/

class Board {
    private:
    char squares[8][8];  // simple 8x8 board, use 'P', 'p', 'R', etc.
    bool white_to_move;
    double eval;
    std::forward_list<std::string> continuations;

    public:
    Board(const char* board_str) {
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                squares[i][j] = board_str[i*8 + j];
        white_to_move = board_str[64]=='w';
    }

    void evaluate() {
        // Simple evaluation function (material count)
        double score = 0.0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                char piece = squares[i][j];
                switch (piece) {
                    case 'P': score += 1.0; break;
                    case 'N': score += 3.0; break;
                    case 'B': score += 3.0; break;
                    case 'R': score += 5.0; break;
                    case 'Q': score += 9.0; break;
                    case 'K': score += 0.0; break;
                    case 'p': score -= 1.0; break;
                    case 'n': score -= 3.0; break;
                    case 'b': score -= 3.0; break;
                    case 'r': score -= 5.0; break;
                    case 'q': score -= 9.0; break;
                    case 'k': score -= 0.0; break;
                }
            }
        }
        this->eval = score;
    }

    void generate_legal_moves() {
        if (white_to_move) {
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    char piece = squares[i][j];
                    switch (piece)
                    {
                    case ' ':
                        break;
                    
                    case 'P':
                        if (squares[i-1][j]==' ') { //simple move forward
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i-1]});
                        }
                        if (i==6 && squares[i-2][j]==' ' && squares[i-1][j]==' ') { //double move
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i-2]});
                        }
                        break;
                    
                    case 'N':
                        for (auto& move : knight_moves) {
                            int dest_i = i + move[0];
                            int dest_j = j + move[1];
                            //looping through all 8 knight moves

                            if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                                if (squares[dest_i][dest_j] == ' ' || black_pieces.find(squares[dest_i][dest_j]) != std::string::npos) { //add legal move if destination is empty or has opponent piece
                                    continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                                }
                            }
                        }
                        break;

                    case 'Q': {
                        // Queen moves like both Rook and Bishop
                        // Fall through to Rook case first
                    }
                    case 'R': {
                        int k = i-1;
                        while (k >= 0 && squares[k][j] == ' ') { //create legal move until blocked
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                            --k;}
                        if (black_pieces.find(squares[k][j]) != std::string::npos) { //if opponent piece, add one more move
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = i+1;
                        while (k < 8 && squares[k][j] == ' ') { //do the same in other direction
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                            ++k;}
                        if (black_pieces.find(squares[k][j]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = j-1;
                        while (k >= 0 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            --k;}
                        if (black_pieces.find(squares[i][k]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        }
                        k = j+1;
                        while (k < 8 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            ++k;}
                        if (black_pieces.find(squares[i][k]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        }
                        if (piece != 'Q') break;  // Create fall through for queen
                    }
                    case 'B': {
                        int ki, kj;
                        ki = i-1; kj = j-1;
                        while (ki >= 0 && kj >= 0 && squares[ki][kj] == ' ') { //create legal move until blocked
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            --ki; --kj;}
                        if (ki >= 0 && kj >= 0 && black_pieces.find(squares[ki][kj]) != std::string::npos) { //if opponent piece, add one more move (and check whether still in bound)
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i-1; kj = j+1;
                        while (ki >= 0 && kj < 8 && squares[ki][kj] == ' ') { //do the same in other direction
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            --ki; ++kj;}
                        if (black_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j-1;
                        while (ki < 8 && kj >= 0 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; --kj;}
                        if (black_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j+1;
                        while (ki < 8 && kj < 8 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; ++kj;}
                        if (black_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        break;
                    }

                    case 'K':
                        for (int di = -1; di <= 1; ++di) {
                            for (int dj = -1; dj <= 1; ++dj) {
                                if (di == 0 && dj == 0) continue;
                                int dest_i = i + di;
                                int dest_j = j + dj;
                                if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                                    if (squares[dest_i][dest_j] == ' ' || black_pieces.find(squares[dest_i][dest_j]) != std::string::npos) {
                                        continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        } else {
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    char piece = squares[i][j];
                    switch (piece)
                    {
                    case ' ':
                        break;
                    
                    case 'p':
                        // simple one-step forward
                        if (i+1 < 8 && squares[i+1][j]==' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i+1]});
                        }
                        // two-step from starting rank (rank 7 for black pawns, i == 1)
                        if (i==1 && squares[i+1][j]==' ' && i+2 < 8 && squares[i+2][j]==' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[i+2]});
                        }
                        // captures
                        if (i+1 < 8 && j-1 >= 0) {
                            char target = squares[i+1][j-1];
                            if (white_pieces.find(target) != std::string::npos) {
                                continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j-1], row_indeces[i+1]});
                            }
                        }
                        if (i+1 < 8 && j+1 < 8) {
                            char target = squares[i+1][j+1];
                            if (white_pieces.find(target) != std::string::npos) {
                                continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j+1], row_indeces[i+1]});
                            }
                        }
                        break;
                    
                    case 'n':
                        for (auto& move : knight_moves) {
                            int dest_i = i + move[0];
                            int dest_j = j + move[1];
                            if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                                if (squares[dest_i][dest_j] == ' ' || white_pieces.find(squares[dest_i][dest_j]) != std::string::npos) {
                                    continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                                }
                            }
                        }
                        break;

                    case 'q': {
                        // Queen moves like both Rook and Bishop — handle via fall-through style but keep safe bounds check
                    }
                    case 'r': {
                        int k = i-1;
                        while (k >= 0 && squares[k][j] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                            --k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[k][j]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = i+1;
                        while (k < 8 && squares[k][j] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                            ++k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[k][j]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = j-1;
                        while (k >= 0 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            --k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[i][k]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        }
                        k = j+1;
                        while (k < 8 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            ++k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[i][k]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        }
                        if (piece != 'q') break; // fall through for queen
                    }
                    case 'b': {
                        int ki, kj;
                        ki = i-1; kj = j-1;
                        while (ki >= 0 && kj >= 0 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            --ki; --kj;
                        }
                        if (ki >= 0 && kj >= 0 && ki < 8 && kj < 8 && white_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i-1; kj = j+1;
                        while (ki >= 0 && kj < 8 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            --ki; ++kj;
                        }
                        if (ki >= 0 && kj >= 0 && ki < 8 && kj < 8 && white_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j-1;
                        while (ki < 8 && kj >= 0 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; --kj;
                        }
                        if (ki >= 0 && kj >= 0 && ki < 8 && kj < 8 && white_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j+1;
                        while (ki < 8 && kj < 8 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; ++kj;
                        }
                        if (ki >= 0 && kj >= 0 && ki < 8 && kj < 8 && white_pieces.find(squares[ki][kj]) != std::string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        break;
                    }

                    case 'k':
                        for (int di = -1; di <= 1; ++di) {
                            for (int dj = -1; dj <= 1; ++dj) {
                                if (di == 0 && dj == 0) continue;
                                int dest_i = i + di;
                                int dest_j = j + dj;
                                if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                                    if (squares[dest_i][dest_j] == ' ' || white_pieces.find(squares[dest_i][dest_j]) != std::string::npos) {
                                        continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[dest_j], row_indeces[dest_i]});
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            } 
        }
    }
};

const char* get_move(const char* board_str) {
    Board board(board_str);

    // TEST CODE
    board.generate_legal_moves();

    //print moves for testing
    /*for (const std::string& move : board.continuations) {
        std::cout << "Generated move: " << move << std::endl;
    }

    return board.continuations.front().c_str();*/
    return "e2e4";
}

// Simple test main for local testing. Builds a starting-position board string
// (64 chars, row-major from rank 8 to rank 1) and appends a side-to-move
// character ('w' or 'b') at index 64. Calls get_move() and prints the result.
int main() {
    std::string start =
        "rnbqkbnr"  // rank 8
        "pppppppp"  // rank 7
        "        "  // rank 6
        "        "  // rank 5
        "        "  // rank 4
        "        "  // rank 3
        "PPPPPPPP"  // rank 2
        "RNBQKBNR"  // rank 1
        "w";         // side to move: 'w' for white, 'b' for black

    const char* result = get_move(start.c_str());
    std::cout << "get_move returned: " << (result ? result : "(null)") << std::endl;
    return 0;
}