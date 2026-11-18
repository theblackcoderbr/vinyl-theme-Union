// SPDX-License-Identifier: GPL-2.0-or-later
#include <QApplication>
#include <QHBoxLayout>
#include <QImage>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStyleOptionMenuItem>
#include <QTextStream>
#include <QTimer>
#include <QWidgetAction>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (QStandardPaths::locate(QStandardPaths::GenericDataLocation,
            "union/styles/org.ekaaty.vinyl/metadata.json").isEmpty()) return 2;
    auto *style = QStyleFactory::create("Union");
    if (!style) return 2;
    app.setStyle(style);

    QPixmap icon(16, 16);
    icon.fill(Qt::red);
    QMenu menu;
    menu.addAction(QIcon(icon), "Properties")->setShortcut(QKeySequence("Alt+Return"));
    auto *row = new QWidget;
    auto *layout = new QHBoxLayout(row);
    QList<QPushButton *> buttons;
    // Dolphin's folder-color plugin uses QPushButtons inside a QWidgetAction,
    // rather than QToolButtons or Quick's display="icon-only" controls.
    for (int i = 0; i < 5; ++i) {
        auto *button = new QPushButton;
        button->setIcon(QIcon(icon));
        layout->addWidget(button);
        buttons.append(button);
    }
    auto *other = new QPushButton("Other");
    other->setMenu(new QMenu(other));
    other->menu()->addAction("Custom");
    layout->addWidget(other);
    auto *action = new QWidgetAction(&menu);
    action->setDefaultWidget(row);
    menu.addAction(action);
    menu.addAction("Terminal");
    menu.popup(QPoint(10, 10));

    QTimer::singleShot(150, [&] {
        bool passed = true;
        QTextStream out(stdout);
        const auto background = menu.grab().toImage().pixelColor(3, 3);
        for (auto *button : buttons) {
            const auto hint = button->sizeHint();
            const auto surface = button->grab().toImage().pixelColor(12, 10);
            const bool compact = hint.width() <= hint.height() + 4;
            const bool distinct = surface != background;
            out << "icon button=" << hint.width() << 'x' << hint.height()
                << " compact=" << compact << " distinct=" << distinct << '\n';
            passed &= compact && distinct;
        }

        const auto textStart = [&](bool withIcon) {
            QStyleOptionMenuItem option;
            option.initFrom(&menu);
            option.rect = QRect(0, 0, 300, 34);
            option.menuItemType = QStyleOptionMenuItem::Normal;
            option.text = "Terminal";
            option.font = menu.font();
            option.maxIconWidth = 16;
            if (withIcon) option.icon = QIcon(icon);
            QImage image(option.rect.size(), QImage::Format_ARGB32_Premultiplied);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            style->drawControl(QStyle::CE_MenuItem, &option, &painter, &menu);
            painter.end();
            // Text is achromatic in both test palettes; the solid red icon is
            // excluded. The unselected item has no background of its own.
            for (int x = 0; x < image.width(); ++x) {
                for (int y = 0; y < image.height(); ++y) {
                    const auto color = image.pixelColor(x, y);
                    if (color.alpha() > 127 && color.red() == color.green()
                        && color.green() == color.blue()) return x;
                }
            }
            return -1;
        };
        const int withIcon = textStart(true);
        const int withoutIcon = textStart(false);
        // Union's Widgets output omits the icon column for iconless entries.
        // Record the remaining difference; CSS cannot add its missing Icon
        // layout element, and this test must not claim that gap is fixed.
        out << "text with icon=" << withIcon << " without icon=" << withoutIcon
            << " (Widgets output difference)" << Qt::endl;
        app.exit(passed ? 0 : 1);
    });
    return app.exec();
}
