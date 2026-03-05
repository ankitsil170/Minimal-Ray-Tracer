#include <QApplication>
#include <QSurfaceFormat>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("RayTracer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("github/yourname");

    // Dark palette
    QPalette dark;
    dark.setColor(QPalette::Window,          QColor(30, 30, 46));
    dark.setColor(QPalette::WindowText,      QColor(205, 214, 244));
    dark.setColor(QPalette::Base,            QColor(24, 24, 37));
    dark.setColor(QPalette::AlternateBase,   QColor(49, 50, 68));
    dark.setColor(QPalette::ToolTipBase,     QColor(30, 30, 46));
    dark.setColor(QPalette::ToolTipText,     QColor(205, 214, 244));
    dark.setColor(QPalette::Text,            QColor(205, 214, 244));
    dark.setColor(QPalette::Button,          QColor(49, 50, 68));
    dark.setColor(QPalette::ButtonText,      QColor(205, 214, 244));
    dark.setColor(QPalette::BrightText,      Qt::red);
    dark.setColor(QPalette::Highlight,       QColor(137, 180, 250));
    dark.setColor(QPalette::HighlightedText, QColor(30, 30, 46));
    app.setPalette(dark);

    MainWindow w;
    w.show();
    return app.exec();
}
