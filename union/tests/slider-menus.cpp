// SPDX-License-Identifier: GPL-2.0-or-later
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    if (argc != 2 || QStandardPaths::locate(QStandardPaths::GenericDataLocation,
            "union/styles/org.ekaaty.vinyl/metadata.json").isEmpty()) {
        QTextStream(stderr) << "Install Vinyl (Union) and provide the test QML path.\n";
        return 2;
    }
    QQuickStyle::setStyle("org.kde.union");
    QQmlApplicationEngine engine;
    engine.load(QUrl::fromLocalFile(QString::fromLocal8Bit(argv[1])));
    if (engine.rootObjects().isEmpty()) return 2;

    QTimer::singleShot(250, [&] {
        auto *root = engine.rootObjects().first();
        QTextStream(stdout) << root->property("report").toString() << Qt::endl;
        app.exit(root->property("passed").toBool() ? 0 : 1);
    });
    return app.exec();
}
