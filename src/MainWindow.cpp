#include "MainWindow.h"
#include "PgnParser.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QBuffer>
#include <QPainter>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QDialog>
#include <QAbstractButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Schachbrett-Editor");
    setMinimumSize(820, 640);
    resize(980, 700);
    setupUI();
    setupMenuBar();
    onPositionChanged();
}

void MainWindow::setupMenuBar() {
    auto *dateiMenu = menuBar()->addMenu("&Datei");
    dateiMenu->addAction("Bild &exportieren...", QKeySequence("Ctrl+E"), this, &MainWindow::onExportImage);
    dateiMenu->addAction("Bild in &Zwischenablage", QKeySequence("Ctrl+Shift+C"), this, &MainWindow::onCopyToClipboard);
    dateiMenu->addSeparator();
    dateiMenu->addAction("&Beenden", QKeySequence::Quit, this, &QWidget::close);

    auto *bearbeitenMenu = menuBar()->addMenu("B&earbeiten");
    bearbeitenMenu->addAction("&FEN laden...", QKeySequence("Ctrl+F"), this, &MainWindow::onLoadFEN);
    bearbeitenMenu->addAction("&PGN laden...", QKeySequence("Ctrl+P"), this, &MainWindow::onLoadPGN);
    bearbeitenMenu->addAction("FEN &kopieren", QKeySequence("Ctrl+C"), this, &MainWindow::onCopyFEN);
    bearbeitenMenu->addSeparator();
    bearbeitenMenu->addAction("&Grundstellung", QKeySequence("Ctrl+R"), this, &MainWindow::onResetBoard);
    bearbeitenMenu->addAction("Brett &leeren", QKeySequence("Ctrl+L"), this, &MainWindow::onClearBoard);

    auto *ansichtMenu = menuBar()->addMenu("&Ansicht");
    ansichtMenu->addAction("Brett &drehen", QKeySequence("F"), this, &MainWindow::onFlipBoard);
}

void MainWindow::setupUI() {
    auto *central = new QWidget;
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(12, 8, 12, 8);

    // Links: Brett
    auto *boardLayout = new QVBoxLayout;
    m_boardWidget = new ChessBoardWidget;
    connect(m_boardWidget, &ChessBoardWidget::positionChanged,
            this, &MainWindow::onPositionChanged);
    boardLayout->addWidget(m_boardWidget, 1);

    // FEN-Leiste
    auto *fenLayout = new QHBoxLayout;
    fenLayout->addWidget(new QLabel("FEN:"));
    m_fenEdit = new QLineEdit;
    m_fenEdit->setFont(QFont("Monospace", 9));
    connect(m_fenEdit, &QLineEdit::returnPressed, this, &MainWindow::onLoadFEN);
    fenLayout->addWidget(m_fenEdit, 1);
    auto *loadFenBtn = new QPushButton("Laden");
    connect(loadFenBtn, &QPushButton::clicked, this, &MainWindow::onLoadFEN);
    fenLayout->addWidget(loadFenBtn);
    auto *copyFenBtn = new QPushButton("Kopieren");
    connect(copyFenBtn, &QPushButton::clicked, this, &MainWindow::onCopyFEN);
    fenLayout->addWidget(copyFenBtn);
    boardLayout->addLayout(fenLayout);

    mainLayout->addLayout(boardLayout, 1);

    // Rechts: Steuerung
    auto *rightPanel = new QVBoxLayout;
    rightPanel->setSpacing(8);
    rightPanel->addWidget(createPiecePalette());
    rightPanel->addWidget(createControlPanel());
    rightPanel->addStretch();

    mainLayout->addLayout(rightPanel, 0);
    setCentralWidget(central);

    m_statusLabel = new QLabel(QString::fromUtf8(
        "Bereit \u2014 Klicken Sie auf Felder, um Figuren zu setzen. Rechtsklick entfernt."));
    statusBar()->addWidget(m_statusLabel);
}

QWidget* MainWindow::createPiecePalette() {
    auto *group = new QGroupBox("Figurenauswahl");
    auto *grid = new QGridLayout(group);
    grid->setSpacing(4);

    m_pieceGroup = new QButtonGroup(this);
    m_pieceGroup->setExclusive(true);
    connect(m_pieceGroup, &QButtonGroup::buttonClicked,
            this, [this](QAbstractButton *btn) { onPieceSelected(m_pieceGroup->id(btn)); });

    struct PieceInfo { char code; QString symbol; QString tooltip; };
    PieceInfo whitePieces[] = {
        {'K', "\u2654", "Wei\u00dfer K\u00f6nig"},
        {'Q', "\u2655", "Wei\u00dfe Dame"},
        {'R', "\u2656", "Wei\u00dfer Turm"},
        {'B', "\u2657", "Wei\u00dfer L\u00e4ufer"},
        {'N', "\u2658", "Wei\u00dfer Springer"},
        {'P', "\u2659", "Wei\u00dfer Bauer"},
    };
    PieceInfo blackPieces[] = {
        {'k', "\u265A", "Schwarzer K\u00f6nig"},
        {'q', "\u265B", "Schwarze Dame"},
        {'r', "\u265C", "Schwarzer Turm"},
        {'b', "\u265D", "Schwarzer L\u00e4ufer"},
        {'n', "\u265E", "Schwarzer Springer"},
        {'p', "\u265F", "Schwarzer Bauer"},
    };

    grid->addWidget(new QLabel(QString::fromUtf8("<b>Wei\u00df</b>")), 0, 0, 1, 6, Qt::AlignCenter);
    for (int i = 0; i < 6; ++i) {
        auto *btn = new QToolButton;
        btn->setText(whitePieces[i].symbol);
        btn->setToolTip(whitePieces[i].tooltip);
        btn->setFont(QFont("", 22));
        btn->setFixedSize(44, 44);
        btn->setCheckable(true);
        m_pieceGroup->addButton(btn, whitePieces[i].code);
        grid->addWidget(btn, 1, i);
    }

    grid->addWidget(new QLabel("<b>Schwarz</b>"), 2, 0, 1, 6, Qt::AlignCenter);
    for (int i = 0; i < 6; ++i) {
        auto *btn = new QToolButton;
        btn->setText(blackPieces[i].symbol);
        btn->setToolTip(blackPieces[i].tooltip);
        btn->setFont(QFont("", 22));
        btn->setFixedSize(44, 44);
        btn->setCheckable(true);
        m_pieceGroup->addButton(btn, blackPieces[i].code);
        grid->addWidget(btn, 3, i);
    }

    // Radierer
    auto *eraserBtn = new QToolButton;
    eraserBtn->setText("\u2716");
    eraserBtn->setToolTip("Radierer \u2014 Klicken entfernt Figuren");
    eraserBtn->setFont(QFont("", 18));
    eraserBtn->setFixedSize(44, 44);
    eraserBtn->setCheckable(true);
    eraserBtn->setChecked(true);
    m_pieceGroup->addButton(eraserBtn, '.');
    grid->addWidget(eraserBtn, 4, 0, 1, 2, Qt::AlignLeft);

    auto *eraserLabel = new QLabel("Radierer");
    eraserLabel->setStyleSheet("color: #666;");
    grid->addWidget(eraserLabel, 4, 2, 1, 4, Qt::AlignLeft | Qt::AlignVCenter);

    return group;
}

QWidget* MainWindow::createControlPanel() {
    auto *group = new QGroupBox("Aktionen && Export");
    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(6);

    auto *actionsLayout = new QGridLayout;

    auto *resetBtn = new QPushButton("Grundstellung");
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetBoard);
    actionsLayout->addWidget(resetBtn, 0, 0);

    auto *clearBtn = new QPushButton("Brett leeren");
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearBoard);
    actionsLayout->addWidget(clearBtn, 0, 1);

    auto *flipBtn = new QPushButton("Brett drehen");
    connect(flipBtn, &QPushButton::clicked, this, &MainWindow::onFlipBoard);
    actionsLayout->addWidget(flipBtn, 1, 0);

    auto *pgnBtn = new QPushButton("PGN laden...");
    connect(pgnBtn, &QPushButton::clicked, this, &MainWindow::onLoadPGN);
    actionsLayout->addWidget(pgnBtn, 1, 1);

    layout->addLayout(actionsLayout);

    auto *line1 = new QFrame;
    line1->setFrameShape(QFrame::HLine);
    layout->addWidget(line1);

    // Farbschema
    auto *schemeLayout = new QHBoxLayout;
    schemeLayout->addWidget(new QLabel("Brettfarben:"));
    m_colorSchemeCombo = new QComboBox;
    m_colorSchemeCombo->addItems({
        "Klassisch (Holz)", "Blau", "Gr\u00fcn", "Violett",
        "Grau", "Braun (dunkel)", "Hoher Kontrast"
    });
    connect(m_colorSchemeCombo, &QComboBox::currentIndexChanged,
            this, &MainWindow::onColorSchemeChanged);
    schemeLayout->addWidget(m_colorSchemeCombo, 1);
    layout->addLayout(schemeLayout);

    auto *line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    layout->addWidget(line2);

    // Export-Einstellungen
    auto *exportLayout = new QGridLayout;
    exportLayout->addWidget(new QLabel("Format:"), 0, 0);
    m_formatCombo = new QComboBox;
    m_formatCombo->addItems({"PNG", "JPEG", "BMP"});
    exportLayout->addWidget(m_formatCombo, 0, 1);

    exportLayout->addWidget(new QLabel(QString::fromUtf8("Gr\u00f6\u00dfe (px):")), 1, 0);
    m_sizeSpin = new QSpinBox;
    m_sizeSpin->setRange(200, 4000);
    m_sizeSpin->setValue(800);
    m_sizeSpin->setSingleStep(100);
    exportLayout->addWidget(m_sizeSpin, 1, 1);
    layout->addLayout(exportLayout);

    auto *exportBtnLayout = new QHBoxLayout;
    auto *exportBtn = new QPushButton("Als Datei speichern...");
    exportBtn->setStyleSheet("font-weight: bold;");
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportImage);
    exportBtnLayout->addWidget(exportBtn);

    auto *clipBtn = new QPushButton("In Zwischenablage");
    clipBtn->setStyleSheet("font-weight: bold;");
    connect(clipBtn, &QPushButton::clicked, this, &MainWindow::onCopyToClipboard);
    exportBtnLayout->addWidget(clipBtn);
    layout->addLayout(exportBtnLayout);

    return group;
}

void MainWindow::onPositionChanged() {
    m_fenEdit->setText(m_boardWidget->position().toFEN());
}

void MainWindow::onLoadFEN() {
    QString fen = m_fenEdit->text().trimmed();
    if (fen.isEmpty()) {
        bool ok;
        fen = QInputDialog::getText(this, "FEN laden",
            "FEN-Zeichenkette eingeben:", QLineEdit::Normal, "", &ok);
        if (!ok || fen.isEmpty()) return;
    }

    ChessPosition pos;
    if (pos.fromFEN(fen)) {
        m_boardWidget->setPosition(pos);
        m_statusLabel->setText("FEN erfolgreich geladen.");
    } else {
        QMessageBox::warning(this, QString::fromUtf8("Ung\u00fcltiges FEN"),
            QString::fromUtf8("Die FEN-Zeichenkette konnte nicht gelesen werden. "
            "Bitte \u00fcberpr\u00fcfen Sie das Format."));
    }
}

void MainWindow::onLoadPGN() {
    QDialog dlg(this);
    dlg.setWindowTitle("PGN laden");
    dlg.resize(520, 420);
    auto *layout = new QVBoxLayout(&dlg);

    layout->addWidget(new QLabel(QString::fromUtf8("PGN-Text hier einf\u00fcgen:")));
    auto *textEdit = new QTextEdit;
    textEdit->setFont(QFont("Monospace", 10));
    textEdit->setPlaceholderText(
        "[Event \"Beispiel\"]\n[Site \"?\"]\n\n1. e4 e5 2. Nf3 Nc6 3. Bb5 *");
    layout->addWidget(textEdit, 1);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("Laden");
    buttons->button(QDialogButtonBox::Cancel)->setText("Abbrechen");
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);

    if (dlg.exec() != QDialog::Accepted) return;

    QString pgn = textEdit->toPlainText().trimmed();
    if (pgn.isEmpty()) return;

    ChessPosition pos;
    bool ok = PgnParser::parse(pgn, pos);
    m_boardWidget->setPosition(pos);

    if (ok) {
        PgnGame game = PgnParser::parseGame(pgn);
        m_statusLabel->setText(QString::fromUtf8("PGN geladen \u2014 %1 Z\u00fcge angewandt.")
                               .arg(game.moves.size()));
    } else {
        m_statusLabel->setText(QString::fromUtf8(
            "PGN teilweise geladen \u2014 einige Z\u00fcge konnten nicht angewandt werden."));
    }
}

void MainWindow::onCopyFEN() {
    QApplication::clipboard()->setText(m_boardWidget->position().toFEN());
    m_statusLabel->setText("FEN in Zwischenablage kopiert.");
}

QImage MainWindow::getExportImage() {
    return m_boardWidget->renderToImage(m_sizeSpin->value());
}

void MainWindow::onExportImage() {
    QString format = m_formatCombo->currentText();
    QString filter;
    if (format == "PNG")  filter = "PNG-Bild (*.png)";
    else if (format == "JPEG") filter = "JPEG-Bild (*.jpg *.jpeg)";
    else if (format == "BMP")  filter = "BMP-Bild (*.bmp)";

    QString path = QFileDialog::getSaveFileName(this, "Schachdiagramm exportieren",
        QString("schachdiagramm.%1").arg(format.toLower()), filter);
    if (path.isEmpty()) return;

    QImage img = getExportImage();
    QString fmt = format.toUpper();
    if (img.save(path, fmt.toLatin1().constData())) {
        m_statusLabel->setText(QString("Exportiert als %1: %2").arg(format, path));
    } else {
        QMessageBox::warning(this, "Export fehlgeschlagen",
            QString::fromUtf8("Das Bild konnte nicht gespeichert werden. "
            "Bitte \u00fcberpr\u00fcfen Sie den Dateipfad und die Berechtigungen."));
    }
}

void MainWindow::onCopyToClipboard() {
    QString format = m_formatCombo->currentText();
    QImage img = getExportImage();

    auto *mimeData = new QMimeData;
    mimeData->setImageData(img);

    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    const char *fmt = format == "JPEG" ? "JPG" : format.toLatin1().constData();
    img.save(&buffer, fmt);
    buffer.close();

    QString mimeType = "image/" + format.toLower();
    if (format == "JPEG") mimeType = "image/jpeg";
    mimeData->setData(mimeType, buffer.data());

    QApplication::clipboard()->setMimeData(mimeData);

    m_statusLabel->setText(QString::fromUtf8("Bild als %1 in Zwischenablage kopiert (%2\u00d7%2 px).")
                           .arg(format).arg(m_sizeSpin->value()));
}

void MainWindow::onResetBoard() {
    m_boardWidget->position().setStartingPosition();
    m_boardWidget->setPosition(m_boardWidget->position());
    m_statusLabel->setText("Grundstellung wiederhergestellt.");
}

void MainWindow::onClearBoard() {
    m_boardWidget->position().clear();
    m_boardWidget->setPosition(m_boardWidget->position());
    m_statusLabel->setText("Brett geleert.");
}

void MainWindow::onFlipBoard() {
    m_boardWidget->setFlipped(!m_boardWidget->isFlipped());
    m_statusLabel->setText(m_boardWidget->isFlipped()
        ? "Brett gedreht (Sicht von Schwarz)."
        : QString::fromUtf8("Brett normal (Sicht von Wei\u00df)."));
}

void MainWindow::onPieceSelected(int id) {
    m_boardWidget->setSelectedPiece(static_cast<char>(id));
    if (id == '.') {
        m_statusLabel->setText(QString::fromUtf8(
            "Radierer gew\u00e4hlt \u2014 Klicken Sie auf Felder, um Figuren zu entfernen."));
    } else {
        QString name;
        bool white = ChessPosition::isWhitePiece(id);
        char upper = std::toupper(id);
        switch (upper) {
            case 'K': name = QString::fromUtf8("K\u00f6nig"); break;
            case 'Q': name = "Dame"; break;
            case 'R': name = "Turm"; break;
            case 'B': name = QString::fromUtf8("L\u00e4ufer"); break;
            case 'N': name = "Springer"; break;
            case 'P': name = "Bauer"; break;
        }
        m_statusLabel->setText(QString::fromUtf8("%1 %2 gew\u00e4hlt \u2014 Klicken zum Setzen.")
                               .arg(white ? QString::fromUtf8("Wei\u00dfer") : "Schwarzer", name));
    }
}

void MainWindow::onColorSchemeChanged(int index) {
    struct ColorScheme { QColor light; QColor dark; QColor border; };
    ColorScheme schemes[] = {
        {{240, 217, 181}, {181, 136, 99},  {60, 40, 20}},     // Klassisch
        {{222, 235, 250}, {100, 140, 200}, {40, 60, 100}},    // Blau
        {{235, 245, 220}, {118, 170, 100}, {40, 70, 30}},     // Gruen
        {{232, 220, 245}, {150, 110, 180}, {60, 40, 80}},     // Violett
        {{230, 230, 230}, {160, 160, 160}, {50, 50, 50}},     // Grau
        {{210, 180, 140}, {139, 90, 43},   {40, 25, 10}},     // Braun
        {{255, 255, 255}, {0, 0, 0},       {80, 80, 80}},     // Hoher Kontrast
    };

    if (index >= 0 && index < 7) {
        m_boardWidget->setLightColor(schemes[index].light);
        m_boardWidget->setDarkColor(schemes[index].dark);
        m_boardWidget->setBorderColor(schemes[index].border);
    }
}
