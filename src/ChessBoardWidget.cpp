#include "ChessBoardWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <cmath>

ChessBoardWidget::ChessBoardWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(280, 280);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    loadPieceSvgs();
}

void ChessBoardWidget::loadPieceSvgs() {
#ifdef HAS_QT_SVG
    QMap<char, QString> paths = {
        {'K', ":/pieces/wK"}, {'Q', ":/pieces/wQ"}, {'R', ":/pieces/wR"},
        {'B', ":/pieces/wB"}, {'N', ":/pieces/wN"}, {'P', ":/pieces/wP"},
        {'k', ":/pieces/bK"}, {'q', ":/pieces/bQ"}, {'r', ":/pieces/bR"},
        {'b', ":/pieces/bB"}, {'n', ":/pieces/bN"}, {'p', ":/pieces/bP"},
    };

    for (auto it = paths.constBegin(); it != paths.constEnd(); ++it) {
        if (QFile::exists(it.value())) {
            auto renderer = std::make_shared<QSvgRenderer>(it.value());
            if (renderer->isValid()) {
                m_svgPieces[it.key()] = renderer;
            }
        }
    }
    m_svgLoaded = !m_svgPieces.isEmpty();
#endif
}

void ChessBoardWidget::setPosition(const ChessPosition &pos) {
    m_position = pos;
    update();
    emit positionChanged();
}

void ChessBoardWidget::setSelectedPiece(char piece) {
    m_selectedPiece = piece;
}

void ChessBoardWidget::setFlipped(bool flipped) {
    m_flipped = flipped;
    update();
}

int ChessBoardWidget::borderSize() const {
    return squareSize() / 2;
}

int ChessBoardWidget::squareSize() const {
    int side = std::min(width(), height());
    return (side - 2) / 9;
}

std::pair<int,int> ChessBoardWidget::squareAt(const QPoint &pos) const {
    int sq = squareSize();
    int border = borderSize();
    int totalBoardPx = 8 * sq + 2 * border;

    int offsetX = (width() - totalBoardPx) / 2;
    int offsetY = (height() - totalBoardPx) / 2;

    int bx = pos.x() - offsetX - border;
    int by = pos.y() - offsetY - border;

    if (bx < 0 || by < 0 || bx >= 8 * sq || by >= 8 * sq)
        return {-1, -1};

    int file = bx / sq;
    int rank = by / sq;

    if (m_flipped) {
        file = 7 - file;
        rank = 7 - rank;
    }

    return {rank, file};
}

QString ChessBoardWidget::unicodePiece(char piece) {
    switch (piece) {
        case 'K': return QString::fromUtf8("\u2654");
        case 'Q': return QString::fromUtf8("\u2655");
        case 'R': return QString::fromUtf8("\u2656");
        case 'B': return QString::fromUtf8("\u2657");
        case 'N': return QString::fromUtf8("\u2658");
        case 'P': return QString::fromUtf8("\u2659");
        case 'k': return QString::fromUtf8("\u265A");
        case 'q': return QString::fromUtf8("\u265B");
        case 'r': return QString::fromUtf8("\u265C");
        case 'b': return QString::fromUtf8("\u265D");
        case 'n': return QString::fromUtf8("\u265E");
        case 'p': return QString::fromUtf8("\u265F");
        default: return "";
    }
}

void ChessBoardWidget::drawPiece(QPainter &painter, char piece, const QRect &rect) const {
#ifdef HAS_QT_SVG
    if (m_svgLoaded && m_svgPieces.contains(piece)) {
        int margin = rect.width() / 20;
        QRect pieceRect = rect.adjusted(margin, margin, -margin, -margin);
        m_svgPieces[piece]->render(&painter, QRectF(pieceRect));
        return;
    }
#endif
    // Unicode-Fallback
    QString symbol = unicodePiece(piece);
    QFont font;
    font.setPixelSize(rect.width() * 0.82);
    painter.setFont(font);

    painter.setPen(QColor(0, 0, 0, 40));
    painter.drawText(rect.adjusted(1, 1, 1, 1), Qt::AlignCenter, symbol);
    painter.setPen(Qt::black);
    painter.drawText(rect, Qt::AlignCenter, symbol);
}

void ChessBoardWidget::drawBoard(QPainter &painter, int totalSize, int offsetX, int offsetY) const {
    int sq = totalSize / 9;
    int border = sq / 2;
    int boardPixels = 8 * sq;
    int actualTotal = boardPixels + 2 * border;

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    // Rahmen
    painter.fillRect(offsetX, offsetY, actualTotal, actualTotal, m_borderColor);

    // Felder und Figuren
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            int displayR = m_flipped ? (7 - r) : r;
            int displayF = m_flipped ? (7 - f) : f;

            int x = offsetX + border + displayF * sq;
            int y = offsetY + border + displayR * sq;

            bool isLight = (r + f) % 2 == 0;
            painter.fillRect(x, y, sq, sq, isLight ? m_lightColor : m_darkColor);

            char piece = m_position.pieceAt(r, f);
            if (ChessPosition::isPiece(piece)) {
                drawPiece(painter, piece, QRect(x, y, sq, sq));
            }
        }
    }

    // Koordinaten
    QFont coordFont;
    coordFont.setPixelSize(border * 0.7);
    coordFont.setBold(true);
    painter.setFont(coordFont);
    painter.setPen(QColor(200, 190, 170));

    for (int i = 0; i < 8; ++i) {
        int displayI = m_flipped ? (7 - i) : i;

        // Linien (a-h)
        QString fileLabel = QString(QChar('a' + i));
        int fx = offsetX + border + displayI * sq;
        painter.drawText(QRect(fx, offsetY + border + boardPixels, sq, border),
                         Qt::AlignCenter, fileLabel);
        painter.drawText(QRect(fx, offsetY, sq, border), Qt::AlignCenter, fileLabel);

        // Reihen (1-8)
        QString rankLabel = QString::number(8 - i);
        int ry = offsetY + border + displayI * sq;
        painter.drawText(QRect(offsetX, ry, border, sq), Qt::AlignCenter, rankLabel);
        painter.drawText(QRect(offsetX + border + boardPixels, ry, border, sq),
                         Qt::AlignCenter, rankLabel);
    }
}

void ChessBoardWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    int side = std::min(width(), height());
    int sq = side / 9;
    int border = sq / 2;
    int totalSize = 8 * sq + 2 * border;

    int offsetX = (width() - totalSize) / 2;
    int offsetY = (height() - totalSize) / 2;

    drawBoard(painter, totalSize, offsetX, offsetY);
}

void ChessBoardWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        auto [rank, file] = squareAt(event->pos());
        if (rank < 0 || file < 0) return;

        if (m_selectedPiece == '.') {
            m_position.removePiece(rank, file);
        } else {
            if (m_position.pieceAt(rank, file) == m_selectedPiece) {
                m_position.removePiece(rank, file);
            } else {
                m_position.setPiece(rank, file, m_selectedPiece);
            }
        }
        update();
        emit positionChanged();
    } else if (event->button() == Qt::RightButton) {
        auto [rank, file] = squareAt(event->pos());
        if (rank >= 0 && file >= 0) {
            m_position.removePiece(rank, file);
            update();
            emit positionChanged();
        }
    }
}

QImage ChessBoardWidget::renderToImage(int size) const {
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    drawBoard(painter, size, 0, 0);
    return image;
}
