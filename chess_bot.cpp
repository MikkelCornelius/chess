#include <string>
#include <forward_list>
#include <vector>
#include <memory>

using namespace std;

// compile with: g++ -shared -fPIC -o chessbot.dll chess_bot.cpp

constexpr char col_indeces[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
constexpr char row_indeces[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

string white_pieces = "PNBRQK";
string black_pieces = "pnbrqk";
int knight_moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                            {1, -2}, {1, 2}, {2, -1}, {2, 1}};

class Board {
    private:
    char squares[8][8];
    string previous_move;
    string best_continuation;
    bool white_to_move;
    double eval;
    forward_list<string> continuations;
    int num_continuations = 0;

    public:
    Board(const char* init_squares, const string& previous_move, bool init_white_to_move, int depth) {
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                squares[i][j] = init_squares[i*8 + j];
        this->previous_move = previous_move;
        this->best_continuation = previous_move;
        this->white_to_move = init_white_to_move;

        if (depth > 1) {
            evaluate(); //write what to do later
        } else if (depth == 1) {
            this->generate_legal_moves();


            // Create array of pointers to boards
            vector<unique_ptr<Board>> boards;
            int length = static_cast<int>(distance(continuations.begin(), continuations.end()));
            boards.reserve(length);

            // Create all boards and store pointers
            int i = 0;
            for (const string& current_move : continuations) {
                char* buf = move(current_move);
                boards.emplace_back(make_unique<Board>(buf, current_move, !white_to_move, depth-1));
                delete [] buf;

                boards.back()->evaluate(); //evaluate boards of depth 0
                i++;
            }

            // Find best continuation
            double board_eval;
            bool first_child = true; //always run *if block* in first iteration to initialize eval
            if (white_to_move) {
                for (const auto& board : boards) {
                    board_eval = board->get_evaluation();
                    if (first_child) {
                        this->eval = board_eval;
                        this->best_continuation = board->get_best_continuation();
                        first_child = false;
                    } else if (board_eval > this->eval) {
                        this->eval = board_eval;
                        this->best_continuation = board->get_best_continuation();
                    }
                }
            } else {
                for (const auto& board : boards) {
                    board_eval = board->get_evaluation();
                    if (first_child) {
                        this->eval = board_eval;
                        this->best_continuation = board->get_best_continuation();
                        first_child = false;
                    } else if (board_eval < this->eval) {
                        this->eval = board_eval;
                        this->best_continuation = board->get_best_continuation();
                    }
                }
            }
        }
    }

    char* move(string current_move) {
        // Create copy of board
        char* new_board = new char[64];
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                new_board[i*8 + j] = squares[i][j];
        
        // Execute move on new_board using ASCII subtraction
        int from_col = current_move[0] - 'a';
        int from_row = '8' - current_move[1];
        int to_col = current_move[2] - 'a';
        int to_row = '8' - current_move[3];
        new_board[to_row * 8 + to_col] = new_board[from_row * 8 + from_col];
        new_board[from_row * 8 + from_col] = ' ';
        
        return new_board;
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
                                if (squares[dest_i][dest_j] == ' ' || black_pieces.find(squares[dest_i][dest_j]) != string::npos) { //add legal move if destination is empty or has opponent piece
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
                        if (k >= 0 && black_pieces.find(squares[k][j]) != string::npos) { //if opponent piece, add one more move (and check whether still in bound)
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = i+1;
                        while (k < 8 && squares[k][j] == ' ') { //do the same in other direction
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                            ++k;}
                        if (k < 8 && black_pieces.find(squares[k][j]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = j-1;
                        while (k >= 0 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            --k;}
                        if (k >= 0 && black_pieces.find(squares[i][k]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        }
                        k = j+1;
                        while (k < 8 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            ++k;}
                        if (k < 8 && black_pieces.find(squares[i][k]) != string::npos) {
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
                        if (ki >= 0 && kj >= 0 && black_pieces.find(squares[ki][kj]) != string::npos) { //if opponent piece, add one more move (and check whether still in bound)
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i-1; kj = j+1;
                        while (ki >= 0 && kj < 8 && squares[ki][kj] == ' ') { //do the same in other direction
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            --ki; ++kj;}
                        if (ki >= 0 && kj < 8 && black_pieces.find(squares[ki][kj]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j-1;
                        while (ki < 8 && kj >= 0 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; --kj;}
                        if (ki < 8 && kj >= 0 && black_pieces.find(squares[ki][kj]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j+1;
                        while (ki < 8 && kj < 8 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; ++kj;}
                        if (ki < 8 && kj < 8 && black_pieces.find(squares[ki][kj]) != string::npos) {
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
                                    if (squares[dest_i][dest_j] == ' ' || black_pieces.find(squares[dest_i][dest_j]) != string::npos) {
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
                            if (white_pieces.find(target) != string::npos) {
                                continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j-1], row_indeces[i+1]});
                            }
                        }
                        if (i+1 < 8 && j+1 < 8) {
                            char target = squares[i+1][j+1];
                            if (white_pieces.find(target) != string::npos) {
                                continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j+1], row_indeces[i+1]});
                            }
                        }
                        break;
                    
                    case 'n':
                        for (auto& move : knight_moves) {
                            int dest_i = i + move[0];
                            int dest_j = j + move[1];
                            if (dest_i >= 0 && dest_i < 8 && dest_j >= 0 && dest_j < 8) {
                                if (squares[dest_i][dest_j] == ' ' || white_pieces.find(squares[dest_i][dest_j]) != string::npos) {
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
                        if (k >= 0 && k < 8 && white_pieces.find(squares[k][j]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = i+1;
                        while (k < 8 && squares[k][j] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                            ++k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[k][j]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[j], row_indeces[k]});
                        }
                        k = j-1;
                        while (k >= 0 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            --k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[i][k]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                        }
                        k = j+1;
                        while (k < 8 && squares[i][k] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[k], row_indeces[i]});
                            ++k;
                        }
                        if (k >= 0 && k < 8 && white_pieces.find(squares[i][k]) != string::npos) {
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
                        if (ki >= 0 && kj >= 0 && white_pieces.find(squares[ki][kj]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i-1; kj = j+1;
                        while (ki >= 0 && kj < 8 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            --ki; ++kj;
                        }
                        if (ki >= 0 && kj < 8 && white_pieces.find(squares[ki][kj]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j-1;
                        while (ki < 8 && kj >= 0 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; --kj;
                        }
                        if (kj >= 0 && ki < 8 && white_pieces.find(squares[ki][kj]) != string::npos) {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                        }
                        ki = i+1; kj = j+1;
                        while (ki < 8 && kj < 8 && squares[ki][kj] == ' ') {
                            continuations.push_front({col_indeces[j], row_indeces[i], col_indeces[kj], row_indeces[ki]});
                            ++ki; ++kj;
                        }
                        if (ki < 8 && kj < 8 && white_pieces.find(squares[ki][kj]) != string::npos) {
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
                                    if (squares[dest_i][dest_j] == ' ' || white_pieces.find(squares[dest_i][dest_j]) != string::npos) {
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

    double get_evaluation() const {
        return eval;
    }

    string get_best_continuation() const {
        return best_continuation;
    }

    string get_previous_move() const {
        return previous_move;
    }
};

extern "C" const char* get_move(const char* board_str) {
    if (!board_str) return nullptr;

    char squares[8][8];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            squares[i][j] = board_str[i*8 + j];
        }
    }
    bool white_to_move = (board_str[64] == 'w');

    int depth = 1;

    Board board(&squares[0][0], string(), white_to_move, depth);

    static string result_storage;
    result_storage = board.get_best_continuation();
    return result_storage.c_str();
}

/*const char* get_move(const char* board_str) {
    if (!board_str) return nullptr;

    char squares[8][8];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            squares[i][j] = board_str[i*8 + j];
        }
    }
    bool white_to_move = (board_str[64] == 'w');

    int depth = 1;
    // pass pointer to first element of the 2D array (flat 64-byte buffer)
    // pass an explicit empty string instead of nullptr to avoid constructing
    // a string from a null pointer (which throws).
    Board board(&squares[0][0], string(), white_to_move, depth);
    //board.generate_legal_moves();

    //print moves for testing
    for (const string& move : board.continuations) {
        cout << "Generated move: " << move << endl;
    }

    return board.continuations.front().c_str();
    // Copy the result into a static string so the returned const char* remains
    // valid after this function returns. (Caller should treat it as read-only
    // and that it may be overwritten by subsequent calls.)
    static string result_storage;
    result_storage = board.get_best_continuation();
    return result_storage.c_str();
}
*/

// Simple test main for local testing. Builds a starting-position board string
// (64 chars, row-major from rank 8 to rank 1) and appends a side-to-move
// character ('w' or 'b') at index 64. Calls get_move() and prints the result.
/*int main() {
    string start =
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
    //cout << "get_move returned: " << (result ? result : "(null)") << endl;
    return 0;
}*/