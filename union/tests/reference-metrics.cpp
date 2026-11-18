// SPDX-License-Identifier: GPL-2.0-or-later
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QPluginLoader>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStylePlugin>
#include <QStyleOption>
#include <QTextStream>
#include <QVBoxLayout>

// Load the reference by absolute filename, so an older desktop installation
// cannot silently replace the plugin built from the source being compared.
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (argc != 2 || !QFileInfo(QString::fromLocal8Bit(argv[1])).isAbsolute()
        || QStandardPaths::locate(QStandardPaths::GenericDataLocation,
            "union/styles/org.ekaaty.vinyl/metadata.json").isEmpty()) {
        QTextStream(stderr) << "Usage: reference-metrics /absolute/path/to/vinyl6.so\n"
                            << "Install Vinyl (Union) before running this comparison.\n";
        return 2;
    }
    QPluginLoader loader(QString::fromLocal8Bit(argv[1]));
    auto *factory = qobject_cast<QStylePlugin *>(loader.instance());
    if (!factory) {
        QTextStream(stderr) << loader.errorString() << Qt::endl;
        return 2;
    }
    auto *native = factory->create("Vinyl");
    auto *unionStyle = QStyleFactory::create("Union");
    if (!native || !unionStyle) {
        delete native;
        delete unionStyle;
        return 2;
    }

    const auto font = app.font();
    const auto measure = [&](QStyle *style) {
        app.setStyle(style);
        app.setFont(font);
        QJsonObject result;
        QWidget gallery;
        auto *layout = new QVBoxLayout(&gallery);
        const auto add = [&](const char *key, QWidget *control) {
            layout->addWidget(control);
            control->ensurePolished();
            const auto hint = control->sizeHint();
            result[key] = QJsonArray{hint.width(), hint.height()};
        };
        auto *checkbox = new QCheckBox("Check");
        add("checkbox", checkbox);
        auto *radio = new QRadioButton("Radio");
        add("radio", radio);
        add("button", new QPushButton("Button"));
        auto *combo = new QComboBox;
        combo->addItems({"First item", "Second item"});
        add("combobox", combo);
        auto *spin = new QSpinBox;
        spin->setRange(0, 999);
        add("spinbox", spin);
        auto *doubleSpin = new QDoubleSpinBox;
        doubleSpin->setRange(0, 999);
        add("doubleSpinbox", doubleSpin);
        add("textfield", new QLineEdit("Text"));
        auto *slider = new QSlider(Qt::Horizontal);
        add("slider", slider);
        auto *progress = new QProgressBar;
        progress->setValue(50);
        add("progress", progress);

        // Union needs the option and widget to identify a control. Null queries
        // for many PM_* metrics fall back to QCommonStyle and do not describe
        // the actual control, so measure the indicator/handle rectangles instead.
        const auto dimensions = [](const QRect &rect) {
            return QJsonArray{rect.width(), rect.height()};
        };
        QJsonObject geometry;
        QStyleOptionButton checkOption;
        checkOption.initFrom(checkbox);
        geometry["checkboxIndicator"] = dimensions(style->subElementRect(QStyle::SE_CheckBoxIndicator, &checkOption, checkbox));
        QStyleOptionButton radioOption;
        radioOption.initFrom(radio);
        radioOption.text = radio->text();
        geometry["radioIndicator"] = dimensions(style->subElementRect(QStyle::SE_RadioButtonIndicator, &radioOption, radio));
        const auto indicator = style->subElementRect(QStyle::SE_RadioButtonIndicator, &radioOption, radio);
        const auto contents = style->subElementRect(QStyle::SE_RadioButtonContents, &radioOption, radio);
        result["radioLabelGap"] = contents.left() - indicator.right() - 1;
        QStyleOptionSlider sliderOption;
        sliderOption.initFrom(slider);
        sliderOption.orientation = Qt::Horizontal;
        sliderOption.minimum = 0;
        sliderOption.maximum = 100;
        sliderOption.sliderPosition = 50;
        sliderOption.rect = QRect(0, 0, 200, slider->sizeHint().height());
        geometry["sliderHandle"] = dimensions(style->subControlRect(QStyle::CC_Slider, &sliderOption, QStyle::SC_SliderHandle, slider));
        QScrollBar scrollbar(Qt::Vertical);
        scrollbar.ensurePolished();
        geometry["scrollbarExtent"] = scrollbar.sizeHint().width();
        result["geometry"] = geometry;
        return result;
    };

    const auto reference = measure(native);
    const auto port = measure(unionStyle);
    const auto referenceGeometry = reference["geometry"].toObject();
    const auto portGeometry = port["geometry"].toObject();
    QJsonObject checks;
    bool passed = true;
    for (auto it = referenceGeometry.begin(); it != referenceGeometry.end(); ++it) {
        const bool match = it.value() == portGeometry[it.key()];
        checks[it.key()] = match;
        passed &= match;
    }
    for (const auto *key : {"checkbox", "button", "textfield", "slider", "progress"}) {
        const bool match = reference[key] == port[key];
        checks[key] = match;
        passed &= match;
    }
    const bool comboHeight = reference["combobox"].toArray()[1] == port["combobox"].toArray()[1];
    checks["comboboxHeight"] = comboHeight;
    passed &= comboHeight;
    // The wider radio label gap is intentional: leave room beyond the surface
    // and its shadow, while preserving the native height and indicator size.
    const bool radioHeight = reference["radio"].toArray()[1] == port["radio"].toArray()[1];
    const bool radioGap = port["radioLabelGap"].toInt() >= 6;
    checks["radioHeight"] = radioHeight;
    checks["radioLabelGap"] = radioGap;
    passed &= radioHeight && radioGap;
    const QJsonObject report{{"font", font.toString()}, {"reference", loader.fileName()},
                             {"native", reference}, {"union", port}, {"checks", checks}};
    QTextStream(stdout) << QJsonDocument(report).toJson(QJsonDocument::Indented);
    return passed ? 0 : 1;
}
