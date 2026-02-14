#pragma once
#include "ChessBoardWidget.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QToolButton>
#include <QButtonGroup>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onPositionChanged();
    void onLoadFEN();
    void onLoadPGN();
    void onCopyFEN();
    void onExportImage();
    void onCopyToClipboard();
    void onResetBoard();
    void onClearBoard();
    void onFlipBoard();
    void onPieceSelected(int id);
    void onColorSchemeChanged(int index);

private:
    void setupUI();
    void setupMenuBar();
    QWidget* createPiecePalette();
    QWidget* createControlPanel();
    QImage getExportImage();

    ChessBoardWidget *m_boardWidget;
    QLineEdit *m_fenEdit;
    QComboBox *m_formatCombo;
    QSpinBox  *m_sizeSpin;
    QComboBox *m_colorSchemeCombo;
    QLabel    *m_statusLabel;
    QButtonGroup *m_pieceGroup;
};
