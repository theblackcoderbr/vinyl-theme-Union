// SPDX-License-Identifier: GPL-2.0-or-later
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStyleOptionButton>
#include <QTextStream>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                              "union/styles/org.ekaaty.vinyl/metadata.json").isEmpty()) {
        QTextStream(stderr) << "Install Vinyl (Union) before running this test.\n";
        return 2;
    }
    auto *style = QStyleFactory::create("Union");
    if (!style) return 2;
    app.setStyle(style);
    QPushButton button;
    button.setFlat(true);
    button.setCheckable(true);
    button.resize(100, 41);
    button.ensurePolished();

    const auto surface = [&](bool checked, bool enabled) {
        QStyleOptionButton option;
        option.initFrom(&button);
        option.rect = button.rect();
        option.features = QStyleOptionButton::Flat;
        option.state = QStyle::State_Active;
        if (enabled) option.state |= QStyle::State_Enabled;
        if (checked) option.state |= QStyle::State_On;
        QImage image(button.size(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        style->drawPrimitive(QStyle::PE_PanelButtonCommand, &option, &painter, &button);
        painter.end();
        return image.pixelColor(image.width() / 2, image.height() / 2);
    };

    const auto unchecked = surface(false, true);
    const auto checked = surface(true, true);
    const auto disabled = surface(true, false);
    QTextStream(stdout) << "flat unchecked=" << unchecked.name(QColor::HexArgb)
                        << " checked=" << checked.name(QColor::HexArgb)
                        << " disabled=" << disabled.name(QColor::HexArgb) << Qt::endl;
    return unchecked.alpha() == 0 && checked.alpha() > 0 && disabled.alpha() == 0 ? 0 : 1;
}
