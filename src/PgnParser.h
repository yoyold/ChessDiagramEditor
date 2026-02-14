#pragma once
#include "ChessPosition.h"
#include <QString>
#include <QStringList>
#include <QMap>

struct PgnGame {
    QMap<QString, QString> headers;
    QStringList moves;
    QString result;
};

class PgnParser {
public:
    static bool parse(const QString &pgn, ChessPosition &outPosition);
    static PgnGame parseGame(const QString &pgn);
    static QStringList extractMoves(const QString &pgn);
};
