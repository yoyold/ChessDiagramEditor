#include "ChessPosition.h"
#include <QStringList>
#include <cctype>

ChessPosition::ChessPosition() { setStartingPosition(); }

void ChessPosition::clear() {
    for (auto &rank : board)
        rank.fill('.');
    whiteToMove = true;
    whiteCastleK = whiteCastleQ = true;
    blackCastleK = blackCastleQ = true;
    enPassantFile = -1;
    halfmoveClock = 0;
    fullmoveNumber = 1;
}

void ChessPosition::setStartingPosition() {
    fromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char ChessPosition::pieceAt(int rank, int file) const {
    if (rank < 0 || rank > 7 || file < 0 || file > 7) return '.';
    return board[rank][file];
}

void ChessPosition::setPiece(int rank, int file, char piece) {
    if (rank >= 0 && rank <= 7 && file >= 0 && file <= 7)
        board[rank][file] = piece;
}

void ChessPosition::removePiece(int rank, int file) {
    setPiece(rank, file, '.');
}

bool ChessPosition::isWhitePiece(char p) {
    return p >= 'A' && p <= 'Z' && QString("KQRBNP").contains(p);
}

bool ChessPosition::isBlackPiece(char p) {
    return p >= 'a' && p <= 'z' && QString("kqrbnp").contains(p);
}

bool ChessPosition::isPiece(char p) {
    return isWhitePiece(p) || isBlackPiece(p);
}

QString ChessPosition::toFEN() const {
    QString fen;
    for (int r = 0; r < 8; ++r) {
        int empty = 0;
        for (int f = 0; f < 8; ++f) {
            if (board[r][f] == '.') {
                ++empty;
            } else {
                if (empty > 0) { fen += QString::number(empty); empty = 0; }
                fen += board[r][f];
            }
        }
        if (empty > 0) fen += QString::number(empty);
        if (r < 7) fen += '/';
    }
    fen += whiteToMove ? " w " : " b ";

    QString castling;
    if (whiteCastleK) castling += 'K';
    if (whiteCastleQ) castling += 'Q';
    if (blackCastleK) castling += 'k';
    if (blackCastleQ) castling += 'q';
    if (castling.isEmpty()) castling = "-";
    fen += castling;

    if (enPassantFile >= 0 && enPassantFile <= 7) {
        int epRank = whiteToMove ? 2 : 5;
        fen += QString(" %1%2").arg(QChar('a' + enPassantFile)).arg(8 - epRank);
    } else {
        fen += " -";
    }

    fen += QString(" %1 %2").arg(halfmoveClock).arg(fullmoveNumber);
    return fen;
}

bool ChessPosition::fromFEN(const QString &fen) {
    QStringList parts = fen.simplified().split(' ');
    if (parts.size() < 1) return false;

    clear();
    QStringList ranks = parts[0].split('/');
    if (ranks.size() != 8) return false;

    for (int r = 0; r < 8; ++r) {
        int f = 0;
        for (QChar ch : ranks[r]) {
            if (ch.isDigit()) {
                f += ch.digitValue();
            } else {
                if (f >= 8) return false;
                board[r][f++] = ch.toLatin1();
            }
        }
        if (f != 8) return false;
    }

    if (parts.size() >= 2) whiteToMove = (parts[1] == "w");

    whiteCastleK = whiteCastleQ = blackCastleK = blackCastleQ = false;
    if (parts.size() >= 3 && parts[2] != "-") {
        for (QChar c : parts[2]) {
            if (c == 'K') whiteCastleK = true;
            if (c == 'Q') whiteCastleQ = true;
            if (c == 'k') blackCastleK = true;
            if (c == 'q') blackCastleQ = true;
        }
    }

    enPassantFile = -1;
    if (parts.size() >= 4 && parts[3] != "-" && parts[3].size() >= 2) {
        enPassantFile = parts[3][0].toLatin1() - 'a';
    }

    if (parts.size() >= 5) halfmoveClock = parts[4].toInt();
    if (parts.size() >= 6) fullmoveNumber = parts[5].toInt();

    return true;
}

void ChessPosition::findPiece(char piece, std::vector<std::pair<int,int>> &positions) const {
    positions.clear();
    for (int r = 0; r < 8; ++r)
        for (int f = 0; f < 8; ++f)
            if (board[r][f] == piece)
                positions.emplace_back(r, f);
}

bool ChessPosition::isSquareAttacked(int rank, int file, bool byWhite) const {
    char knight = byWhite ? 'N' : 'n';
    int knightMoves[][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
    for (auto &m : knightMoves) {
        int r = rank + m[0], f = file + m[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8 && board[r][f] == knight) return true;
    }

    char bishop = byWhite ? 'B' : 'b';
    char queen  = byWhite ? 'Q' : 'q';
    int diagDirs[][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
    for (auto &d : diagDirs) {
        for (int i = 1; i < 8; ++i) {
            int r = rank + d[0]*i, f = file + d[1]*i;
            if (r < 0 || r > 7 || f < 0 || f > 7) break;
            if (board[r][f] == bishop || board[r][f] == queen) return true;
            if (board[r][f] != '.') break;
        }
    }

    char rook = byWhite ? 'R' : 'r';
    int straightDirs[][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    for (auto &d : straightDirs) {
        for (int i = 1; i < 8; ++i) {
            int r = rank + d[0]*i, f = file + d[1]*i;
            if (r < 0 || r > 7 || f < 0 || f > 7) break;
            if (board[r][f] == rook || board[r][f] == queen) return true;
            if (board[r][f] != '.') break;
        }
    }

    if (byWhite) {
        if (rank + 1 <= 7) {
            if (file - 1 >= 0 && board[rank+1][file-1] == 'P') return true;
            if (file + 1 <= 7 && board[rank+1][file+1] == 'P') return true;
        }
    } else {
        if (rank - 1 >= 0) {
            if (file - 1 >= 0 && board[rank-1][file-1] == 'p') return true;
            if (file + 1 <= 7 && board[rank-1][file+1] == 'p') return true;
        }
    }

    char king = byWhite ? 'K' : 'k';
    for (int dr = -1; dr <= 1; ++dr)
        for (int df = -1; df <= 1; ++df) {
            if (dr == 0 && df == 0) continue;
            int r = rank + dr, f = file + df;
            if (r >= 0 && r < 8 && f >= 0 && f < 8 && board[r][f] == king) return true;
        }

    return false;
}

bool ChessPosition::applyMove(const QString &san) {
    QString move = san.trimmed();
    while (move.endsWith('+') || move.endsWith('#') || move.endsWith('!') || move.endsWith('?'))
        move.chop(1);
    if (move.isEmpty()) return false;

    if (tryApplyCastling(move)) {
        whiteToMove = !whiteToMove;
        if (whiteToMove) fullmoveNumber++;
        return true;
    }
    if (tryApplyPawnMove(move)) {
        whiteToMove = !whiteToMove;
        if (whiteToMove) fullmoveNumber++;
        return true;
    }
    if (tryApplyPieceMove(move)) {
        whiteToMove = !whiteToMove;
        if (whiteToMove) fullmoveNumber++;
        return true;
    }
    return false;
}

bool ChessPosition::tryApplyCastling(const QString &move) {
    if (move == "O-O" || move == "0-0") {
        if (whiteToMove) {
            board[7][4] = '.'; board[7][5] = 'R'; board[7][6] = 'K'; board[7][7] = '.';
            whiteCastleK = whiteCastleQ = false;
        } else {
            board[0][4] = '.'; board[0][5] = 'r'; board[0][6] = 'k'; board[0][7] = '.';
            blackCastleK = blackCastleQ = false;
        }
        enPassantFile = -1;
        return true;
    }
    if (move == "O-O-O" || move == "0-0-0") {
        if (whiteToMove) {
            board[7][0] = '.'; board[7][2] = 'K'; board[7][3] = 'R'; board[7][4] = '.';
            whiteCastleK = whiteCastleQ = false;
        } else {
            board[0][0] = '.'; board[0][2] = 'k'; board[0][3] = 'r'; board[0][4] = '.';
            blackCastleK = blackCastleQ = false;
        }
        enPassantFile = -1;
        return true;
    }
    return false;
}

bool ChessPosition::tryApplyPawnMove(const QString &move) {
    if (move.isEmpty()) return false;
    QChar first = move[0];
    if (!first.isLower() || first < 'a' || first > 'h') return false;

    int srcFile = first.toLatin1() - 'a';
    bool isCapture = move.contains('x');
    char promotionPiece = 0;

    QString cleanMove = move;
    if (cleanMove.contains('=')) {
        int eqIdx = cleanMove.indexOf('=');
        if (eqIdx + 1 < cleanMove.size()) {
            char pp = cleanMove[eqIdx + 1].toLatin1();
            promotionPiece = whiteToMove ? pp : std::tolower(pp);
        }
        cleanMove = cleanMove.left(eqIdx);
    }

    cleanMove.remove('x');

    if (cleanMove.size() < 2) return false;
    int dstFile = cleanMove[cleanMove.size() - 2].toLatin1() - 'a';
    int dstRankNum = cleanMove[cleanMove.size() - 1].digitValue();
    int dstRank = 8 - dstRankNum;

    if (dstFile < 0 || dstFile > 7 || dstRank < 0 || dstRank > 7) return false;

    char pawn = whiteToMove ? 'P' : 'p';
    int direction = whiteToMove ? 1 : -1;

    int srcRank = -1;
    if (!isCapture) {
        if (pieceAt(dstRank + direction, srcFile) == pawn) {
            srcRank = dstRank + direction;
        } else if (pieceAt(dstRank + 2 * direction, srcFile) == pawn &&
                   pieceAt(dstRank + direction, srcFile) == '.') {
            srcRank = dstRank + 2 * direction;
        }
    } else {
        srcRank = dstRank + direction;
        if (srcRank < 0 || srcRank > 7) return false;
        if (pieceAt(srcRank, srcFile) != pawn) return false;
    }

    if (srcRank < 0 || srcRank > 7) return false;

    bool epCapture = isCapture && pieceAt(dstRank, dstFile) == '.' && dstFile == enPassantFile;

    board[srcRank][srcFile] = '.';
    if (epCapture) {
        board[srcRank][dstFile] = '.';
    }
    board[dstRank][dstFile] = promotionPiece ? promotionPiece : pawn;

    if (std::abs(srcRank - dstRank) == 2) {
        enPassantFile = srcFile;
    } else {
        enPassantFile = -1;
    }

    halfmoveClock = 0;
    return true;
}

bool ChessPosition::tryApplyPieceMove(const QString &move) {
    if (move.isEmpty() || !move[0].isUpper()) return false;

    char pieceChar = move[0].toLatin1();
    if (!whiteToMove) {
        pieceChar = std::tolower(pieceChar);
    }

    QString rest = move.mid(1);
    rest.remove('+').remove('#');
    rest.remove('x');

    if (rest.size() < 2) return false;
    int dstFile = rest[rest.size() - 2].toLatin1() - 'a';
    int dstRankNum = rest[rest.size() - 1].digitValue();
    int dstRank = 8 - dstRankNum;

    if (dstFile < 0 || dstFile > 7 || dstRank < 0 || dstRank > 7) return false;

    int disambigFile = -1, disambigRank = -1;
    QString disambig = rest.left(rest.size() - 2);
    for (QChar c : disambig) {
        if (c >= 'a' && c <= 'h') disambigFile = c.toLatin1() - 'a';
        else if (c >= '1' && c <= '8') disambigRank = 8 - c.digitValue();
    }

    std::vector<std::pair<int,int>> candidates;
    findPiece(pieceChar, candidates);

    std::vector<std::pair<int,int>> valid;
    for (auto &[r, f] : candidates) {
        if (disambigFile >= 0 && f != disambigFile) continue;
        if (disambigRank >= 0 && r != disambigRank) continue;

        char pType = std::toupper(pieceChar);
        bool canReach = false;

        if (pType == 'N') {
            int dr = std::abs(r - dstRank), df = std::abs(f - dstFile);
            canReach = (dr == 2 && df == 1) || (dr == 1 && df == 2);
        } else if (pType == 'B') {
            if (std::abs(r - dstRank) == std::abs(f - dstFile)) {
                canReach = true;
                int stepR = (dstRank > r) ? 1 : -1;
                int stepF = (dstFile > f) ? 1 : -1;
                for (int i = 1; i < std::abs(r - dstRank); ++i) {
                    if (board[r + i*stepR][f + i*stepF] != '.') { canReach = false; break; }
                }
            }
        } else if (pType == 'R') {
            if (r == dstRank || f == dstFile) {
                canReach = true;
                if (r == dstRank) {
                    int step = (dstFile > f) ? 1 : -1;
                    for (int i = f + step; i != dstFile; i += step)
                        if (board[r][i] != '.') { canReach = false; break; }
                } else {
                    int step = (dstRank > r) ? 1 : -1;
                    for (int i = r + step; i != dstRank; i += step)
                        if (board[i][f] != '.') { canReach = false; break; }
                }
            }
        } else if (pType == 'Q') {
            if (r == dstRank || f == dstFile) {
                canReach = true;
                if (r == dstRank) {
                    int step = (dstFile > f) ? 1 : -1;
                    for (int i = f + step; i != dstFile; i += step)
                        if (board[r][i] != '.') { canReach = false; break; }
                } else {
                    int step = (dstRank > r) ? 1 : -1;
                    for (int i = r + step; i != dstRank; i += step)
                        if (board[i][f] != '.') { canReach = false; break; }
                }
            }
            if (!canReach && std::abs(r - dstRank) == std::abs(f - dstFile)) {
                canReach = true;
                int stepR = (dstRank > r) ? 1 : -1;
                int stepF = (dstFile > f) ? 1 : -1;
                for (int i = 1; i < std::abs(r - dstRank); ++i) {
                    if (board[r + i*stepR][f + i*stepF] != '.') { canReach = false; break; }
                }
            }
        } else if (pType == 'K') {
            canReach = std::abs(r - dstRank) <= 1 && std::abs(f - dstFile) <= 1;
        }

        if (canReach) valid.emplace_back(r, f);
    }

    if (valid.empty()) return false;
    auto [srcRank, srcFile] = valid[0];

    char upper = std::toupper(pieceChar);
    if (upper == 'K') {
        if (whiteToMove) { whiteCastleK = whiteCastleQ = false; }
        else { blackCastleK = blackCastleQ = false; }
    }
    if (upper == 'R') {
        if (srcRank == 7 && srcFile == 7) whiteCastleK = false;
        if (srcRank == 7 && srcFile == 0) whiteCastleQ = false;
        if (srcRank == 0 && srcFile == 7) blackCastleK = false;
        if (srcRank == 0 && srcFile == 0) blackCastleQ = false;
    }

    bool capture = isPiece(board[dstRank][dstFile]);
    board[srcRank][srcFile] = '.';
    board[dstRank][dstFile] = pieceChar;

    enPassantFile = -1;
    halfmoveClock = capture ? 0 : halfmoveClock + 1;
    return true;
}
