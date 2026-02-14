#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Schachbrett-Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SchachbrettEditor");

    app.setStyleSheet(R"(
        QMainWindow { background: #f5f5f5; }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ccc;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 16px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }
        QPushButton {
            padding: 5px 12px;
            border: 1px solid #aaa;
            border-radius: 3px;
            background: #fff;
        }
        QPushButton:hover { background: #e8e8e8; }
        QPushButton:pressed { background: #d0d0d0; }
        QToolButton {
            border: 2px solid transparent;
            border-radius: 4px;
            background: #fff;
        }
        QToolButton:checked {
            border: 2px solid #4a90d9;
            background: #dce8f5;
        }
        QToolButton:hover { background: #e8e8e8; }
        QComboBox, QSpinBox, QLineEdit {
            padding: 4px;
            border: 1px solid #aaa;
            border-radius: 3px;
        }
    )");

    MainWindow window;
    window.show();

    return app.exec();
}
