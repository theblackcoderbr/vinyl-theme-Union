// SPDX-License-Identifier: GPL-2.0-or-later
import QtQuick
import QtQuick.Controls
import org.kde.union.impl as U

ApplicationWindow {
    visible: true
    width: 640
    height: 480

    function hasSurface(control) {
        return control.U.Style.properties.background.color.a > 0
    }

    readonly property var checks: ({
        flatUnchecked: !hasSurface(unchecked),
        flatChecked: hasSurface(checked),
        flatCheckedDisabled: !hasSurface(disabled),
        widgetsViewFocus: view.U.Style.properties.outline.left.size === 0,
        quickViewFocus: quickView.U.Style.properties.outline.left.size === 0,
        selectedView: hasSurface(selectedView),
        toolbarSeparator: !hasSurface(separator),
        headerSeparator: !hasSurface(headerSeparator),
        standaloneSeparator: hasSurface(standaloneSeparator),
        textfieldHeight: field.implicitHeight === 33,
        comboboxHeight: combo.implicitHeight === 41
    })
    readonly property bool passed: Object.values(checks).every(value => value)
    readonly property string report: JSON.stringify(checks)

    Column {
        x: 40
        y: 40
        Button { id: unchecked; text: "Unchecked"; flat: true; checkable: true }
        Button { id: checked; text: "Checked"; flat: true; checkable: true; checked: true }
        Button { id: disabled; text: "Disabled"; flat: true; checkable: true; checked: true; enabled: false }
        ToolBar {
            ToolSeparator { id: separator }
        }
        ToolSeparator { id: standaloneSeparator }
        TextField { id: field; text: "Text" }
        ComboBox { id: combo; model: ["First item", "Second item"] }
    }
    header: ToolBar {
        ToolSeparator { id: headerSeparator }
    }

    // Widgets' item-view element has no public Quick control. Query its real
    // style rules through Union's attached element, including both focus flags.
    Item {
        id: view
        U.Element.type: "itemviewitem"
        U.Element.states { enabled: true; activeFocus: true; visualFocus: true }
    }
    Item {
        id: quickView
        U.Element.type: "itemdelegate"
        U.Element.states { enabled: true; activeFocus: true; visualFocus: true }
    }
    Item {
        id: selectedView
        U.Element.type: "itemviewitem"
        U.Element.states { enabled: true; highlighted: true; activeFocus: true; visualFocus: true }
    }
}
