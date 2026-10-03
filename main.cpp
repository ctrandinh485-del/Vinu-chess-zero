#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

struct Move {
    int fromR, fromC;
    int toR, toC;
};

int board[8][8] = {
    {-2, -3, -4, -5, -6, -4, -3, -2},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 1,  1,  1,  1,  1,  1,  1,  1},
    { 2,  3,  4,  5,  6,  4,  3,  2}
};

int knightDr[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
int knightDc[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
int rookDr[4] = {-1, 1, 0, 0};
int rookDc[4] = {0, 0, -1, 1};
int bishopDr[4] = {-1, -1, 1, 1};
int bishopDc[4] = {-1, 1, -1, 1};
int kingDr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int kingDc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

char getPieceChar(int p) {
    switch(p) {
        case 1: return 'P'; case -1: return 'p';
        case 2: return 'R'; case -2: return 'r';
        case 3: return 'N'; case -3: return 'n';
        case 4: return 'B'; case -4: return 'b';
        case 5: return 'Q'; case -5: return 'q';
        case 6: return 'K'; case -6: return 'k';
        default: return '.';
    }
}

void printBoard() {
    cout << "\n  a b c d e f g h\n";
    for (int r = 0; r < 8; r++) {
        cout << 8 - r << " ";
        for (int c = 0; c < 8; c++) {
            cout << getPieceChar(board[r][c]) << " ";
        }
        cout << 8 - r << "\n";
    }
    cout << "  a b c d e f g h\n\n";
}

bool isValid(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

vector<Move> getAllMoves(bool isWhite) {
    vector<Move> moves;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int p = board[r][c];
            if ((isWhite && p > 0) || (!isWhite && p < 0)) {
                int pieceType = abs(p);
                if (pieceType == 1) {
                    int dir = isWhite ? -1 : 1;
                    int nr = r + dir;
                    if (isValid(nr, c) && board[nr][c] == 0) moves.push_back(Move{r, c, nr, c});
                    for (int dc : {-1, 1}) {
                        int nc = c + dc;
                        if (isValid(nr, nc)) {
                            if ((isWhite && board[nr][nc] < 0) || (!isWhite && board[nr][nc] > 0)) {
                                moves.push_back(Move{r, c, nr, nc});
                            }
                        }
                    }
                } else if (pieceType == 3) {
                    for (int i = 0; i < 8; i++) {
                        int nr = r + knightDr[i], nc = c + knightDc[i];
                        if (isValid(nr, nc)) {
                            if ((isWhite && board[nr][nc] <= 0) || (!isWhite && board[nr][nc] >= 0))
                                moves.push_back(Move{r, c, nr, nc});
                        }
                    }
                } else if (pieceType == 2 || pieceType == 4 || pieceType == 5) {
                    vector<int> dr, dc;
                    if (pieceType == 2) { dr.assign(rookDr, rookDr+4); dc.assign(rookDc, rookDc+4); }
                    else if (pieceType == 4) { dr.assign(bishopDr, bishopDr+4); dc.assign(bishopDc, bishopDc+4); }
                    else {
                        dr.assign(rookDr, rookDr+4); dr.insert(dr.end(), bishopDr, bishopDr+4);
                        dc.assign(rookDc, rookDc+4); dc.insert(dc.end(), bishopDc, bishopDc+4);
                    }
                    for (size_t i = 0; i < dr.size(); i++) {
                        int nr = r + dr[i], nc = c + dc[i];
                        while (isValid(nr, nc)) {
                            if (board[nr][nc] == 0) {
                                moves.push_back(Move{r, c, nr, nc});
                            } else {
                                if ((isWhite && board[nr][nc] < 0) || (!isWhite && board[nr][nc] > 0))
                                    moves.push_back(Move{r, c, nr, nc});
                                break;
                            }
                            nr += dr[i]; nc += dc[i];
                        }
                    }
                } else if (pieceType == 6) {
                    for (int i = 0; i < 8; i++) {
                        int nr = r + kingDr[i], nc = c + kingDc[i];
                        if (isValid(nr, nc)) {
                            if ((isWhite && board[nr][nc] <= 0) || (!isWhite && board[nr][nc] >= 0))
                                moves.push_back(Move{r, c, nr, nc});
                        }
                    }
                }
            }
        }
    }
    return moves;
}

int evaluateBoard() {
    int score = 0;
    int values[] = {0, 100, 500, 320, 330, 900, 20000};
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int p = board[r][c];
            if (p > 0) score += values[p];
            else if (p < 0) score -= values[-p];
        }
    }
    return score;
}

int minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (depth == 0) return evaluateBoard();
    vector<Move> moves = getAllMoves(isMaximizing);
    if (moves.empty()) return evaluateBoard();

    if (isMaximizing) {
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
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
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
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

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

string toAlgebraic(int r, int c) {
    string s = "";
    s += (char)('a' + c);
    s += to_string(8 - r);
    return s;
}

int main() {
    cout << "=== VINU CHESS ZERO v1.0 ===\n";
    printBoard();

    string input;
    while (true) {
        cout << "Nuoc di cua ban (vd: e2e4) hoac \"quit\": ";
        cin >> input;
        if (input == "quit") break;
        if (input.length() != 4) {
            cout << "Nuoc di khong hop le! Thu lai.\n";
            continue;
        }

        int fromC = input[0] - 'a';
        int fromR = 8 - (input[1] - '0');
        int toC = input[2] - 'a';
        int toR = 8 - (input[3] - '0');

        board[toR][toC] = board[fromR][fromC];
        board[fromR][fromC] = 0;

        printBoard();

        cout << "\nAI (Den) dang suy nghi...\n";
        Move aiMove = getBestMove(false, 3);
        cout << "AI chon nuoc di: " << toAlgebraic(aiMove.fromR, aiMove.fromC) 
             << " -> " << toAlgebraic(aiMove.toR, aiMove.toC) << "\n";

        board[aiMove.toR][aiMove.toC] = board[aiMove.fromR][aiMove.fromC];
        board[aiMove.fromR][aiMove.fromC] = 0;

        printBoard();
    }
    return 0;
}
