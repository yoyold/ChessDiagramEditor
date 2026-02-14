#pragma once
#include "ChessPosition.h"
#include <QWidget>
#include <QImage>
#include <QMap>
#include <memory>

#ifdef HAS_QT_SVG
#include <QSvgRenderer>
#endif

class ChessBoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChessBoardWidget(QWidget *parent = nullptr);

    ChessPosition &position() { return m_position; }
    const ChessPosition &position() const { return m_position; }
    void setPosition(const ChessPosition &pos);

    void setSelectedPiece(char piece);
    char selectedPiece() const { return m_selectedPiece; }

    void setFlipped(bool flipped);
    bool isFlipped() const { return m_flipped; }

    void setLightColor(const QColor &c) { m_lightColor = c; update(); }
    void setDarkColor(const QColor &c) { m_darkColor = c; update(); }
    void setBorderColor(const QColor &c) { m_borderColor = c; update(); }

    QColor lightColor() const { return m_lightColor; }
    QColor darkColor() const { return m_darkColor; }

    QImage renderToImage(int size = 800) const;

signals:
    void positionChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    QSize sizeHint() const override { return QSize(560, 560); }
    QSize minimumSizeHint() const override { return QSize(280, 280); }

private:
    void loadPieceSvgs();
    void drawBoard(QPainter &painter, int totalSize, int offsetX = 0, int offsetY = 0) const;
    void drawPiece(QPainter &painter, char piece, const QRect &rect) const;
    int squareSize() const;
    int borderSize() const;
    std::pair<int,int> squareAt(const QPoint &pos) const;

    // Unicode-Fallback
    static QString unicodePiece(char piece);

    ChessPosition m_position;
    char m_selectedPiece = '.';
    bool m_flipped = false;

    QColor m_lightColor{240, 217, 181};
    QColor m_darkColor{181, 136, 99};
    QColor m_borderColor{60, 40, 20};

    // SVG-Renderer fuer jede Figur (nur mit Qt SVG Modul)
#ifdef HAS_QT_SVG
    QMap<char, std::shared_ptr<QSvgRenderer>> m_svgPieces;
#endif
    bool m_svgLoaded = false;
};
