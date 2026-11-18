// SPDX-License-Identifier: GPL-2.0-or-later

#include <QApplication>
#include <QDialogButtonBox>
#include <QImage>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    if (QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                              "union/styles/org.ekaaty.vinyl/metadata.json").isEmpty()) {
        QTextStream(stderr) << "Install Vinyl (Union) before running this test.\n";
        return 2;
    }
    auto *style = QStyleFactory::create("Union");
    if (!style) return 2;
    app.setStyle(style);
    QWidget window;
    auto *layout = new QVBoxLayout(&window);
    auto *focus = new QLineEdit("Focus outside footer");
    layout->addWidget(focus);
    auto *normal = new QPushButton("Neutral control");
    normal->setAutoDefault(false);
    layout->addWidget(normal);
    auto *box = new QDialogButtonBox;
    auto *help = box->addButton(QDialogButtonBox::Help);
    auto *defaults = box->addButton(QDialogButtonBox::RestoreDefaults);
    help->setAutoDefault(true); defaults->setAutoDefault(true);
    layout->addWidget(box);
    window.resize(500,180); window.show(); focus->setFocus();
    QTimer::singleShot(150, [&] {
        QTextStream out(stdout);
        bool pass = true;
        const auto sample = [](QPushButton *button) { return button->grab().toImage().pixelColor(12,10); };
        auto expected = sample(normal);
        for (auto *button : {help, defaults}) {
            auto actual = sample(button);
            const bool match = expected == actual && !button->hasFocus() && !button->isDefault();
            out << button->text() << " autoDefault=" << button->autoDefault() << " default=" << button->isDefault() << " focus=" << button->hasFocus() << " color=" << actual.name() << " neutral=" << expected.name() << " match=" << match << '\n';
            pass &= match;
        }
        help->setFocus(Qt::TabFocusReason);
        app.processEvents();
        pass &= help->hasFocus() && sample(help) != expected;
        focus->setFocus();
        help->setEnabled(false); normal->setEnabled(false); app.processEvents();
        pass &= sample(help) == sample(normal);
        if(argc>1) window.grab().save(argv[1]);
        app.exit(pass ? 0 : 1);
    });
    return app.exec();
}
