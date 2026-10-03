
#include <iostream>
#include <vector>
#include <string>


using namespace std;

// ==========================================
// VINU CHESS ZERO - FULL PIECE MOVES
// ==========================================

int board[8][8] = {
    {-4, -2, -3, -5, -6, -3, -2, -4},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 1,  1,  1,  1,  1,  1,  1,  1},
    { 4,  2,  3,  5,  6,  3,  2,  4}
};

struct Move {
    int fromR, fromC;
    int toR, toC;
};

// Hướng di chuyển của các quân
int knightDr[8] = {-2, -2, -1, -1,  1,  1,  2,  2};
int knightDc[8] = {-1,  1, -2,  2, -2,  2, -1,  1};

int rookDr[4] = {-1, 1,  0, 0};
int rookDc[4] = { 0, 0, -1, 1};

int bishopDr[4] = {-1, -1, 1, 1};
int bishopDc[4] = {-1,  1,-1, 1};

int kingDr[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int kingDc[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

bool isValid(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

string toAlgebraic(int r, int c) {
    char colChar = 'a' + c;
    char rowChar = '8' - r;
    return string(1, colChar) + string(1, rowChar);
}

// In bàn cờ ra màn hình
void printBoard() {
    cout << "\n  a b c d e f g h\n";
    cout << " +----------------+\n";
    for (int r = 0; r < 8; r++) {
        cout << 8 - r << "|";
        for (int c = 0; c < 8; c++) {
            char p = '.';
            int val = board[r][c];
            if (val == 1) p = 'P'; else if (val == -1) p = 'p';
            else if (val == 2) p = 'N'; else if (val == -2) p = 'n';
            else if (val == 3) p = 'B'; else if (val == -3) p = 'b';
            else if (val == 4) p = 'R'; else if (val == -4) p = 'r';
            else if (val == 5) p = 'Q'; else if (val == -5) p = 'q';
            else if (val == 6) p = 'K'; else if (val == -6) p = 'k';
            cout << p << " ";
        }
        cout << "|" << 8 - r << "\n";
    }
    cout << " +----------------+\n";
    cout << "  a b c d e f g h\n\n";
}

// Sinh nước đi cho TỐT
vector<Move> getPawnMoves(int r, int c, bool isWhite) {
    vector<Move> moves;
    int dir = isWhite ? -1 : 1;
    int startRow = isWhite ? 6 : 1;

    // Tiến 1 ô
    if (isValid(r + dir, c) && board[r + dir][c] == 0) {
        moves.push_back({r, c, r + dir, c});
        // Tiến 2 ô từ vị trí ban đầu
        if (r == startRow && board[r + 2 * dir][c] == 0) {
            moves.push_back({r, c, r + 2 * dir, c});
        }
    }
    // Ăn chéo
    for (int dc : {-1, 1}) {
        int nr = r + dir, nc = c + dc;
        if (isValid(nr, nc)) {
            int target = board[nr][nc];
            if ((isWhite && target < 0) || (!isWhite && target > 0)) {
                moves.push_back({r, c, nr, nc});
            }
        }
    }
        // Sinh nuoc di cho Vua (King)
        if (abs(p) == 6) {
            for (int i = 0; i < 8; i++) {
                int nr = r + kingDr[i];
                int nc = c + kingDc[i];
                if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                    if ((isWhite && board[nr][nc] <= 0) || (!isWhite && board[nr][nc] >= 0)) {
                        moves.push_back({r, c, nr, nc});
                    }
                }
            }
        }
    return moves;
}

// Sinh nước đi cho MÃ
vector<Move> getKnightMoves(int r, int c, bool isWhite) {
    vector<Move> moves;
    for (int i = 0; i < 8; i++) {
        int nr = r + knightDr[i], nc = c + knightDc[i];
        if (isValid(nr, nc)) {
            int target = board[nr][nc];
            if (target == 0 || (isWhite && target < 0) || (!isWhite && target > 0)) {
                moves.push_back({r, c, nr, nc});
            }
        }
    }
        // Sinh nuoc di cho Vua (King)
        if (abs(p) == 6) {
            for (int i = 0; i < 8; i++) {
                int nr = r + kingDr[i];
                int nc = c + kingDc[i];
                if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                    if ((isWhite && board[nr][nc] <= 0) || (!isWhite && board[nr][nc] >= 0)) {
                        moves.push_back({r, c, nr, nc});
                    }
                }
            }
        }
    return moves;
}

// Sinh nước đi trượt (XE, TƯỢNG, HẬU)
vector<Move> getSlidingMoves(int r, int c, bool isWhite, const int dr[], const int dc[], int count) {
    vector<Move> moves;
    for (int i = 0; i < count; i++) {
        int nr = r + dr[i], nc = c + dc[i];
        while (isValid(nr, nc)) {
            int target = board[nr][nc];
            if (target == 0) {
                moves.push_back({r, c, nr, nc});
            } else {
                if ((isWhite && target < 0) || (!isWhite && target > 0)) {
                    moves.push_back({r, c, nr, nc});
                }
                break;
            }
            nr += dr[i];
            nc += dc[i];
        }
    }
        // Sinh nuoc di cho Vua (King)
        if (abs(p) == 6) {
            for (int i = 0; i < 8; i++) {
                int nr = r + kingDr[i];
                int nc = c + kingDc[i];
                if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                    if ((isWhite && board[nr][nc] <= 0) || (!isWhite && board[nr][nc] >= 0)) {
                        moves.push_back({r, c, nr, nc});
                    }
                }
            }
        }
    return moves;
}

// Tổng hợp tất cả nước đi hợp lệ của một bên (Trắng/Đen)
vector<Move> getAllMoves(bool isWhite) {
    vector<Move> allMoves;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int p = board[r][c];
            if ((isWhite && p > 0) || (!isWhite && p < 0)) {
                vector<Move> moves;
                int type = abs(p);
                if (type == 1) moves = getPawnMoves(r, c, isWhite);
                else if (type == 2) moves = getKnightMoves(r, c, isWhite);
                else if (type == 3) moves = getSlidingMoves(r, c, isWhite, bishopDr, bishopDc, 4); // Tượng
                else if (type == 4) moves = getSlidingMoves(r, c, isWhite, rookDr, rookDc, 4);     // Xe
                else if (type == 5) { // Hậu (Xe + Tượng)
                    auto m1 = getSlidingMoves(r, c, isWhite, rookDr, rookDc, 4);
                    auto m2 = getSlidingMoves(r, c, isWhite, bishopDr, bishopDc, 4);
                    moves.insert(moves.end(), m1.begin(), m1.end());
                    moves.insert(moves.end(), m2.begin(), m2.end());
                }
                allMoves.insert(allMoves.end(), moves.begin(), moves.end());
            }
        }
    }
    return allMoves;
}

// 1. Hàm đánh giá thế cờ (Evaluation Function)
int evaluateBoard() {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            score += board[r][c]; // Dương = Trắng ưu thế, Âm = Đen ưu thế
        }
    }
    return score;
}

// 2. Thuật toán Minimax kết hợp Cắt tỉa Alpha-Beta
int minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (depth == 0) {
        return evaluateBoard();
    }

    vector<Move> moves = getAllMoves(isMaximizing);
    if (moves.empty()) return evaluateBoard();

    if (isMaximizing) { // Lượt của Trắng
        int maxEval = -99999;
        for (const auto& move : moves) {
            int temp = board[move.toR][move.toC];
            board[move.toR][move.toC] = board[move.fromR][move.fromC];
            board[move.fromR][move.fromC] = 0;

            int eval = minimax(depth - 1, false, alpha, beta);

            board[move.fromR][move.fromC] = board[move.toR][move.toC];
            board[move.toR][move.toC] = temp;

            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) break; // Cắt tỉa
        }
        return maxEval;
    } else { // Lượt của Đen
        int minEval = 99999;
        for (const auto& move : moves) {
            int temp = board[move.toR][move.toC];
            board[move.toR][move.toC] = board[move.fromR][move.fromC];
            board[move.fromR][move.fromC] = 0;

            int eval = minimax(depth - 1, true, alpha, beta);

            board[move.fromR][move.fromC] = board[move.toR][move.toC];
            board[move.toR][move.toC] = temp;

            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) break; // Cắt tỉa
        }
        return minEval;
    }
}

// 3. Hàm tìm nước đi tốt nhất cho AI
Move getBestMove(bool isWhite, int depth) {
    vector<Move> moves = getAllMoves(isWhite);
    Move bestMove = moves[0];
      int bestValue = isWhite ? -99999 : 99999;

    for (const auto& move : moves) {
        int temp = board[move.toR][move.toC];
        board[move.toR][move.toC] = board[move.fromR][move.fromC];
        board[move.fromR][move.fromC] = 0;

        int boardValue = minimax(depth - 1, !isWhite, -99999, 99999);

        board[move.fromR][move.fromC] = board[move.toR][move.toC];
        board[move.toR][move.toC] = temp;

        if (isWhite) {
            if (boardValue > bestValue) {
                bestValue = boardValue;
                bestMove = move;

            }
        } else {
            if (boardValue < bestValue) {
                bestValue = boardValue;
                bestMove = move;
            }
        }
    }
    return bestMove;
}
int main() {
    cout << "========================================" << endl;
    cout << "          VINU CHESS ZERO v1.0          " << endl;
    cout << "========================================" << endl;
    cout << "Nguoi choi: Trang (In hoa: P, R, N, B, Q, K)" << endl;
    cout << "AI: Den (In thuong: p, r, n, b, q, k)" << endl;
    cout << "Nhap nước đi dang: e2e4 (hoac gõ 'exit' de thoát)\n" << endl;

    bool isGameOver = false;
    string playerInput;

    while (!isGameOver) {
        printBoard();

        // 1. LUOT CỦA NGƯỜI (BÊN TRẮNG)
        cout << "\nLượt của bạn (Trắng): ";
        cin >> playerInput;

        if (playerInput == "exit") {
            cout << "Cảm ơn bạn đã chơi Vinu Chess Zero!\n";
            break;
        }

        if (playerInput.length() != 4) {
            cout << "Lỗi: Nhập sai định dạng! Vui lòng nhập kiểu 'e2e4'.\n";
            continue;
        }

        // Chuyển đổi tọa độ từ ký tự (e2e4 -> row, col)
        int fromC = playerInput[0] - 'a';
        int fromR = 8 - (playerInput[1] - '0');
        int toC   = playerInput[2] - 'a';
        int toR   = 8 - (playerInput[3] - '0');

        // Cập nhật nước đi của Người
        board[toR][toC] = board[fromR][fromC];
        board[fromR][fromC] = 0;

        printBoard();

        // 2. LƯỢT CỦA AI (BÊN ĐEN)
        cout << "\nAI (Đen) đang suy nghĩ nước đi...\n";
        Move aiMove = getBestMove(false, 3); // Độ sâu depth = 3
        
        cout << "AI chọn nước đi: " 
             << toAlgebraic(aiMove.fromR, aiMove.fromC) 
             << " -> " 
             << toAlgebraic(aiMove.toR, aiMove.toC) << "\n\n";

        // Cập nhật nước đi của AI
        board[aiMove.toR][aiMove.toC] = board[aiMove.fromR][aiMove.fromC];
        board[aiMove.fromR][aiMove.fromC] = 0;
    }

    return 0;
}











    
















