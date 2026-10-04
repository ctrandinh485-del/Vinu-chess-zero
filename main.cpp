#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

struct Move {
    int fromR, fromC;
    int toR, toC;
    int promo = 0;
    bool isCastling = false;
    bool isEnPassant = false;
    int score = 0;
};

struct UndoState {
    int savedPiece;
    int savedEpR, savedEpC;
    bool savedWK, savedWQ, savedWRK, savedWRQ;
    bool savedBK, savedBQ, savedBRK, savedBRQ;
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

bool whiteKingMoved = false, whiteRookKingMoved = false, whiteRookQueenMoved = false;
bool blackKingMoved = false, blackRookKingMoved = false, blackRookQueenMoved = false;
int enPassantTargetR = -1, enPassantTargetC = -1;

int knightDr[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
int knightDc[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
int rookDr[4] = {-1, 1, 0, 0};
int rookDc[4] = {0, 0, -1, 1};
int bishopDr[4] = {-1, -1, 1, 1};
int bishopDc[4] = {-1, 1, -1, 1};
int kingDr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int kingDc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

int pieceValues[] = {0, 100, 500, 320, 330, 900, 20000};

int pawnPST[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 27, 27, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

int knightPST[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

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

bool isSquareAttacked(int r, int c, bool attackedByWhite) {
    for (int nr = 0; nr < 8; nr++) {
        for (int nc = 0; nc < 8; nc++) {
            int p = board[nr][nc];
            if ((attackedByWhite && p > 0) || (!attackedByWhite && p < 0)) {
                int piece = abs(p);
                if (piece == 1) {
                    int dir = attackedByWhite ? -1 : 1;
                    if (nr + dir == r && (nc - 1 == c || nc + 1 == c)) return true;
                } else if (piece == 3) {
                    for (int i = 0; i < 8; i++) {
                        if (nr + knightDr[i] == r && nc + knightDc[i] == c) return true;
                    }
                } else if (piece == 2 || piece == 4 || piece == 5) {
                    vector<int> dr, dc;
                    if (piece == 2) { dr.assign(rookDr, rookDr+4); dc.assign(rookDc, rookDc+4); }
                    else if (piece == 4) { dr.assign(bishopDr, bishopDr+4); dc.assign(bishopDc, bishopDc+4); }
                    else {
                        dr.assign(rookDr, rookDr+4); dr.insert(dr.end(), bishopDr, bishopDr+4);
                        dc.assign(rookDc, rookDc+4); dc.insert(dc.end(), bishopDc, bishopDc+4);
                    }
                    for (size_t i = 0; i < dr.size(); i++) {
                        int tr = nr + dr[i], tc = nc + dc[i];
                        while (isValid(tr, tc)) {
                            if (tr == r && tc == c) return true;
                            if (board[tr][tc] != 0) break;
                            tr += dr[i]; tc += dc[i];
                        }
                    }
                } else if (piece == 6) {
                    for (int i = 0; i < 8; i++) {
                        if (nr + kingDr[i] == r && nc + kingDc[i] == c) return true;
                    }
                }
            }
        }
    }
    return false;
}

bool isInCheck(bool isWhite) {
    int kingR = -1, kingC = -1;
    int targetKing = isWhite ? 6 : -6;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (board[r][c] == targetKing) {
                kingR = r; kingC = c;
                break;
            }
        }
        if (kingR != -1) break;
    }
    // FIX: Không tìm thấy Vua -> không bị chiếu (tránh false positive)
    if (kingR == -1) return false;
    return isSquareAttacked(kingR, kingC, !isWhite);
}

vector<Move> getPseudoMoves(bool isWhite) {
    vector<Move> moves;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int p = board[r][c];
            if ((isWhite && p > 0) || (!isWhite && p < 0)) {
                int pieceType = abs(p);
                if (pieceType == 1) {
                    int dir = isWhite ? -1 : 1;
                    int startRow = isWhite ? 6 : 1;
                    int promoRow = isWhite ? 0 : 7;
                    int nr = r + dir;

                    if (isValid(nr, c) && board[nr][c] == 0) {
                        if (nr == promoRow) {
                            moves.push_back(Move{r, c, nr, c, isWhite ? 5 : -5});
                        } else {
                            moves.push_back(Move{r, c, nr, c});
                            int nr2 = r + 2 * dir;
                            if (r == startRow && board[nr2][c] == 0) {
                                moves.push_back(Move{r, c, nr2, c});
                            }
                        }
                    }
                    for (int dc : {-1, 1}) {
                        int nc = c + dc;
                        if (isValid(nr, nc)) {
                            if ((isWhite && board[nr][nc] < 0) || (!isWhite && board[nr][nc] > 0)) {
                                if (nr == promoRow) {
                                    moves.push_back(Move{r, c, nr, nc, isWhite ? 5 : -5});
                                } else {
                                    moves.push_back(Move{r, c, nr, nc});
                                }
                            }
                            if (nr == enPassantTargetR && nc == enPassantTargetC) {
                                moves.push_back(Move{r, c, nr, nc, 0, false, true});
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
                    if (isWhite && !whiteKingMoved && r == 7 && c == 4 && !isInCheck(true)) {
                        if (!whiteRookKingMoved && board[7][7] == 2 && board[7][5] == 0 && board[7][6] == 0) {
                            if (!isSquareAttacked(7, 5, false) && !isSquareAttacked(7, 6, false))
                                moves.push_back(Move{7, 4, 7, 6, 0, true});
                        }
                        if (!whiteRookQueenMoved && board[7][0] == 2 && board[7][1] == 0 && board[7][2] == 0 && board[7][3] == 0) {
                            if (!isSquareAttacked(7, 3, false) && !isSquareAttacked(7, 2, false))
                                moves.push_back(Move{7, 4, 7, 2, 0, true});
                        }
                    } else if (!isWhite && !blackKingMoved && r == 0 && c == 4 && !isInCheck(false)) {
                        if (!blackRookKingMoved && board[0][7] == -2 && board[0][5] == 0 && board[0][6] == 0) {
                            if (!isSquareAttacked(0, 5, true) && !isSquareAttacked(0, 6, true))
                                moves.push_back(Move{0, 4, 0, 6, 0, true});
                        }
                        if (!blackRookQueenMoved && board[0][0] == -2 && board[0][1] == 0 && board[0][2] == 0 && board[0][3] == 0) {
                            if (!isSquareAttacked(0, 3, true) && !isSquareAttacked(0, 2, true))
                                moves.push_back(Move{0, 4, 0, 2, 0, true});
                        }
                    }
                }
            }
        }
    }
    return moves;
}

// FIX: Lưu/restore enPassant + castling flags + en passant capture
void makeMove(const Move& m, UndoState& st) {
    st.savedPiece = board[m.toR][m.toC];
    st.savedEpR = enPassantTargetR;
    st.savedEpC = enPassantTargetC;
    st.savedWK = whiteKingMoved; st.savedWQ = whiteRookQueenMoved; st.savedWRK = whiteRookKingMoved;
    st.savedBK = blackKingMoved; st.savedBQ = blackRookQueenMoved; st.savedBRK = blackRookKingMoved;

    if (m.promo != 0) {
        board[m.toR][m.toC] = m.promo;
    } else {
        board[m.toR][m.toC] = board[m.fromR][m.fromC];
    }
    board[m.fromR][m.fromC] = 0;

    if (m.isEnPassant) {
        int epPawnR = m.fromR;
        int epPawnC = m.toC;
        st.savedPiece = board[epPawnR][epPawnC];
        board[epPawnR][epPawnC] = 0;
    } else if (m.isCastling) {
        if (m.toR == 7 && m.toC == 6) { board[7][5] = board[7][7]; board[7][7] = 0; }
        else if (m.toR == 7 && m.toC == 2) { board[7][3] = board[7][0]; board[7][0] = 0; }
        else if (m.toR == 0 && m.toC == 6) { board[0][5] = board[0][7]; board[0][7] = 0; }
        else if (m.toR == 0 && m.toC == 2) { board[0][3] = board[0][0]; board[0][0] = 0; }
    }

    // Update castling flags ngay trong makeMove
    if (m.fromR == 7 && m.fromC == 4) whiteKingMoved = true;
    if (m.fromR == 7 && m.fromC == 0) whiteRookQueenMoved = true;
    if (m.fromR == 7 && m.fromC == 7) whiteRookKingMoved = true;
    if (m.fromR == 0 && m.fromC == 4) blackKingMoved = true;
    if (m.fromR == 0 && m.fromC == 0) blackRookQueenMoved = true;
    if (m.fromR == 0 && m.fromC == 7) blackRookKingMoved = true;

    // Update en passant target
    if (abs(board[m.toR][m.toC]) == 1 && abs(m.fromR - m.toR) == 2) {
        enPassantTargetR = (m.fromR + m.toR) / 2;
        enPassantTargetC = m.fromC;
    } else {
        enPassantTargetR = -1;
        enPassantTargetC = -1;
    }
}

void undoMove(const Move& m, const UndoState& st) {
    if (m.isEnPassant) {
        board[m.fromR][m.fromC] = board[m.toR][m.toC];
        board[m.toR][m.toC] = 0;
        int epPawnR = m.fromR;
        int epPawnC = m.toC;
        board[epPawnR][epPawnC] = st.savedPiece;
    } else if (m.isCastling) {
        board[m.fromR][m.fromC] = board[m.toR][m.toC];
        board[m.toR][m.toC] = 0;
        if (m.toR == 7 && m.toC == 6) { board[7][7] = board[7][5]; board[7][5] = 0; }
        else if (m.toR == 7 && m.toC == 2) { board[7][0] = board[7][3]; board[7][3] = 0; }
        else if (m.toR == 0 && m.toC == 6) { board[0][7] = board[0][5]; board[0][5] = 0; }
        else if (m.toR == 0 && m.toC == 2) { board[0][0] = board[0][3]; board[0][3] = 0; }
    } else {
        if (m.promo != 0) {
            board[m.fromR][m.fromC] = (m.promo > 0) ? 1 : -1;
        } else {
            board[m.fromR][m.fromC] = board[m.toR][m.toC];
        }
        board[m.toR][m.toC] = st.savedPiece;
    }

    enPassantTargetR = st.savedEpR;
    enPassantTargetC = st.savedEpC;
    whiteKingMoved = st.savedWK; whiteRookQueenMoved = st.savedWQ; whiteRookKingMoved = st.savedWRK;
    blackKingMoved = st.savedBK; blackRookQueenMoved = st.savedBQ; blackRookKingMoved = st.savedBRK;
}

void scoreMoves(vector<Move>& moves, const Move* pvMove = nullptr) {
    for (auto& m : moves) {
        m.score = 0;
        if (pvMove && m.fromR == pvMove->fromR && m.fromC == pvMove->fromC && m.toR == pvMove->toR && m.toC == pvMove->toC) {
            m.score = 20000;
            continue;
        }
        int attacker = abs(board[m.fromR][m.fromC]);
        int victim = abs(board[m.toR][m.toC]);
        if (victim != 0) {
            m.score = 10000 + (pieceValues[victim] * 10) - pieceValues[attacker];
        }
        if (m.promo != 0) {
            m.score += 9000;
        }
    }
    sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

vector<Move> getLegalMoves(bool isWhite, const Move* pvMove = nullptr) {
    vector<Move> pseudo = getPseudoMoves(isWhite);
    vector<Move> legal;
    for (const auto& m : pseudo) {
        UndoState st;
        makeMove(m, st);
        if (!isInCheck(isWhite)) legal.push_back(m);
        undoMove(m, st);
    }
    scoreMoves(legal, pvMove);
    return legal;
}

int evaluateBoard() {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int p = board[r][c];
            if (p > 0) {
                score += pieceValues[p];
                if (p == 1) score += pawnPST[r][c];
                else if (p == 3) score += knightPST[r][c];
            } else if (p < 0) {
                score -= pieceValues[-p];
                if (p == -1) score -= pawnPST[7 - r][c];
                else if (p == -3) score -= knightPST[7 - r][c];
            }
        }
    }
    return score;
}

// FIX: QSearch dùng pseudo moves, check illegal thủ công, tránh corrupt state
int quiescenceSearch(bool isMaximizing, int alpha, int beta) {
    int standPat = evaluateBoard();
    if (isMaximizing) {
        if (standPat >= beta) return beta;
        if (alpha < standPat) alpha = standPat;
    } else {
        if (standPat <= alpha) return alpha;
        if (beta > standPat) beta = standPat;
    }

    vector<Move> moves = getPseudoMoves(isMaximizing);
    scoreMoves(moves);

    for (const auto& move : moves) {
        // Chỉ xét capture/promotion/en passant
        if (board[move.toR][move.toC] == 0 && move.promo == 0 && !move.isEnPassant) continue;

        UndoState st;
        makeMove(move, st);
        if (isInCheck(isMaximizing)) { undoMove(move, st); continue; }
        int score = quiescenceSearch(!isMaximizing, alpha, beta);
        undoMove(move, st);

        if (isMaximizing) {
            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        } else {
            if (score <= alpha) return alpha;
            if (score < beta) beta = score;
        }
    }
    return isMaximizing ? alpha : beta;
}

int minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (depth == 0) return quiescenceSearch(isMaximizing, alpha, beta);

    vector<Move> moves = getLegalMoves(isMaximizing);
    if (moves.empty()) {
        if (isInCheck(isMaximizing)) return isMaximizing ? -90000 : 90000;
        return 0;
    }

    if (isMaximizing) {
        int maxEval = -99999;
        for (const auto& move : moves) {
            UndoState st;
            makeMove(move, st);
            int eval = minimax(depth - 1, false, alpha, beta);
            undoMove(move, st);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 99999;
        for (const auto& move : moves) {
            UndoState st;
            makeMove(move, st);
            int eval = minimax(depth - 1, true, alpha, beta);
            undoMove(move, st);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move getBestMoveIterative(bool isWhite, int maxDepth) {
    Move bestMove = {-1, -1, -1, -1};

    for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++) {
        vector<Move> moves = getLegalMoves(isWhite, (bestMove.fromR != -1) ? &bestMove : nullptr);
        if (moves.empty()) break;

        Move currentBest = moves[0];
        int bestValue = isWhite ? -99999 : 99999;

        for (const auto& move : moves) {
            UndoState st;
            makeMove(move, st);
            int boardValue = minimax(currentDepth - 1, !isWhite, -99999, 99999);
            undoMove(move, st);

            if (isWhite) {
                if (boardValue > bestValue) {
                    bestValue = boardValue;
                    currentBest = move;
                }
            } else {
                if (boardValue < bestValue) {
                    bestValue = boardValue;
                    currentBest = move;
                }
            }
        }
        bestMove = currentBest;
        cout << "  -> Pass Depth " << currentDepth << " hoan thanh!" << endl;
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
    cout << "=== VINU CHESS ZERO v1.7 (FIXED QUIESCENCE SEARCH) ===\n";
    printBoard();

    string input;
    while (true) {
        if (isInCheck(true)) cout << "[CANH BAO] Vua Trang dang bi chieu!\n";
        cout << "Nuoc di cua ban (vd: e2e4) hoac \"quit\": ";
        cin >> input;
        if (input == "quit") break;
        if (input.length() != 4) {
            cout << "Cu phap khong hop le! Thu lai.\n";
            continue;
        }

        int fromC = input[0] - 'a';
        int fromR = 8 - (input[1] - '0');
        int toC = input[2] - 'a';
        int toR = 8 - (input[3] - '0');

        vector<Move> legal = getLegalMoves(true);
        Move selectedMove = {-1, -1, -1, -1};
        for (const auto& m : legal) {
            if (m.fromR == fromR && m.fromC == fromC && m.toR == toR && m.toC == toC) {
                selectedMove = m;
                break;
            }
        }

        if (selectedMove.fromR == -1) {
            cout << "Nuoc di KHONG HOP LE! Thu lai.\n";
            continue;
        }

        UndoState st;
        makeMove(selectedMove, st);
        printBoard();

        cout << "\nAI (Den) dang suy nghi (Depth 5 + Quiescence Search)...\n";
        Move aiMove = getBestMoveIterative(false, 5);
        if (aiMove.fromR == -1) {
            cout << "AI khong con nuoc di hop le! TRO CHOI KET THUC.\n";
            break;
        }
        cout << "AI chon nuoc di: " << toAlgebraic(aiMove.fromR, aiMove.fromC)
             << " -> " << toAlgebraic(aiMove.toR, aiMove.toC) << "\n";

        UndoState st2;
        makeMove(aiMove, st2);
        printBoard();
    }
    return 0;
}
