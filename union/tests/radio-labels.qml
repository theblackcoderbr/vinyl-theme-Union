// SPDX-License-Identifier: GPL-2.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.union.impl as U

ApplicationWindow {
    visible: true
    width: 700
    height: 480

    function gap(control) {
        const label = control.contentItem.children.find(child => child.text !== undefined)
        // Track positions explicitly: mapToItem() alone does not register x/y
        // dependencies when the Positioner updates after the first layout.
        const positions = [label.x, label.y, control.contentItem.x, control.indicator.x]
        const text = label.mapToItem(control, 0, 0)
        const indicator = control.indicator.mapToItem(control, 0, 0)
        return control.mirrored
            ? indicator.x - text.x - label.width
            : text.x - indicator.x - control.indicator.width
    }
    readonly property var checks: ({
        label: gap(normal) >= 6,
        mirroredLabel: gap(mirrored) >= 6,
        constrainedLabel: gap(constrained) >= 6,
        formLabel: gap(formRadio) >= 6,
        checkedEdge: normal.indicator.U.Style.properties.border.left.size >= 1,
        uncheckedEdge: constrained.indicator.U.Style.properties.border.left.size >= 1,
        disabledFocus: disabled.indicator.U.Style.properties.outline.left.size === 0
    })
    readonly property bool passed: Object.values(checks).every(value => value)
    readonly property string report: JSON.stringify({checks: checks, gaps: [gap(normal), gap(mirrored), gap(constrained), gap(formRadio)]})

    ColumnLayout {
        x: 40
        y: 40
        RadioButton { id: normal; text: "Faz com que selecione"; checked: true }
        RadioButton { id: mirrored; text: "Faz com que selecione"; LayoutMirroring.enabled: true }
        RadioButton { id: constrained; text: "Faz com que abra"; Layout.preferredWidth: 100 }
        RadioButton { id: disabled; text: "Desativado"; enabled: false }
        Kirigami.FormLayout {
            ColumnLayout {
                Kirigami.FormData.label: "Um clique em arquivos e pastas:"
                RadioButton { id: formRadio; text: "Faz com que selecione" }
                Label { text: "Para abrir, use clique duplo" }
            }
        }
    }
}
