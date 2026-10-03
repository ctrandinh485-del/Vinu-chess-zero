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

int main() {
    cout << "=======================================\n";
    cout << "        VINU CHESS ZERO v1.0           \n";
    cout << "=======================================\n";

    printBoard();

    // Lấy tất cả nước đi hợp lệ đầu tiên cho phe TRẮNG
    vector<Move> whiteMoves = getAllMoves(true);

    cout << "Tong so nuoc di hop le ban dau cua TRANG: " << whiteMoves.size() << "\n\n";
    cout << "Danh sach cac nuoc di co thể:\n";
    for (const auto& m : whiteMoves) {
        cout << toAlgebraic(m.fromR, m.fromC) << " -> " << toAlgebraic(m.toR, m.toC) << "  ";
    }
    cout << "\n\n";

    return 0;
}

