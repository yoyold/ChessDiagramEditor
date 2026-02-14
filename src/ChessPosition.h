#pragma once
#include <QString>
#include <array>
#include <vector>

// Figurenkodierung: Grossbuchstaben = Weiss, Kleinbuchstaben = Schwarz
// K Q R B N P k q r b n p, '.' = leer
struct ChessPosition {
    std::array<std::array<char, 8>, 8> board{};  // board[reihe][linie], reihe 0 = Reihe 8 (oben)

    bool whiteToMove = true;
    bool whiteCastleK = true, whiteCastleQ = true;
    bool blackCastleK = true, blackCastleQ = true;
    int enPassantFile = -1;
    int halfmoveClock = 0;
    int fullmoveNumber = 1;

    ChessPosition();

    void clear();
    void setStartingPosition();
    char pieceAt(int rank, int file) const;
    void setPiece(int rank, int file, char piece);
    void removePiece(int rank, int file);

    QString toFEN() const;
    bool fromFEN(const QString &fen);

    static bool isWhitePiece(char p);
    static bool isBlackPiece(char p);
    static bool isPiece(char p);

    // Zug in algebraischer Notation anwenden (z.B. "e4", "Sf3", "O-O")
    bool applyMove(const QString &moveStr);

private:
    bool tryApplyPawnMove(const QString &move);
    bool tryApplyCastling(const QString &move);
    bool tryApplyPieceMove(const QString &move);
    void findPiece(char piece, std::vector<std::pair<int,int>> &positions) const;
    bool isSquareAttacked(int rank, int file, bool byWhite) const;
};
