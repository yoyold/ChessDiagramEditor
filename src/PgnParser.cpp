#include "PgnParser.h"
#include <QRegularExpression>

PgnGame PgnParser::parseGame(const QString &pgn) {
    PgnGame game;
    QString text = pgn.trimmed();

    // Header parsen [Key "Value"]
    QRegularExpression headerRe(R"~~(\[(\w+)\s+"([^"]*)"\])~~");
    auto it = headerRe.globalMatch(text);
    int lastHeaderEnd = 0;
    while (it.hasNext()) {
        auto match = it.next();
        game.headers[match.captured(1)] = match.captured(2);
        lastHeaderEnd = match.capturedEnd();
    }

    QString movetext = text.mid(lastHeaderEnd).trimmed();

    // Kommentare { ... } entfernen
    movetext.remove(QRegularExpression(R"(\{[^}]*\})"));
    // Varianten ( ... ) entfernen
    QRegularExpression ravRe(R"(\([^()]*\))");
    while (movetext.contains(ravRe))
        movetext.remove(ravRe);

    // NAGs entfernen
    movetext.remove(QRegularExpression(R"(\$\d+)"));
    // Zugnummern entfernen
    movetext.remove(QRegularExpression(R"(\d+\.+)"));

    // Ergebnis extrahieren
    QRegularExpression resultRe(R"((1-0|0-1|1/2-1/2|\*)\s*$)");
    auto resultMatch = resultRe.match(movetext);
    if (resultMatch.hasMatch()) {
        game.result = resultMatch.captured(1);
        movetext = movetext.left(resultMatch.capturedStart());
    }

    QStringList tokens = movetext.simplified().split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
    for (const QString &token : tokens) {
        QString t = token.trimmed();
        if (!t.isEmpty() && t != "..." && !t[0].isDigit()) {
            game.moves.append(t);
        }
    }

    return game;
}

QStringList PgnParser::extractMoves(const QString &pgn) {
    return parseGame(pgn).moves;
}

bool PgnParser::parse(const QString &pgn, ChessPosition &outPosition) {
    PgnGame game = parseGame(pgn);

    if (game.headers.contains("FEN")) {
        outPosition.fromFEN(game.headers["FEN"]);
    } else {
        outPosition.setStartingPosition();
    }

    for (const QString &move : game.moves) {
        if (!outPosition.applyMove(move)) {
            return false;
        }
    }

    return true;
}
