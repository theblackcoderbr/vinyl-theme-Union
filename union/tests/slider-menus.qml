// SPDX-License-Identifier: GPL-2.0-or-later
import QtQuick
import QtQuick.Controls
import org.kde.union.impl as U
ApplicationWindow {
    id:window
    visible:true; width:640; height:480
    function measure(control) {
        let handle = control.handle.mapToItem(control, control.handle.width / 2, control.handle.height / 2)
        let groove = control.background.mapToItem(control, control.background.width / 2, control.background.height / 2)
        return {horizontal:control.horizontal,delta:control.horizontal ? handle.y-groove.y : handle.x-groove.x,
            handleWidth:control.handle.width,handleHeight:control.handle.height,
            width:control.width,height:control.height}
    }
    readonly property string report:JSON.stringify({
        sliders:[measure(horizontal),measure(vertical),measure(mirrored)],
        menu:{color:String(menu.U.Style.properties.background.color),border:menu.U.Style.properties.border.left.size,
            borderColor:String(menu.U.Style.properties.border.left.color),windowColor:String(window.U.Style.properties.background.color)},
        popup:{color:String(popup.U.Style.properties.background.color),border:popup.U.Style.properties.border.left.size}
    })
    Slider {id:horizontal;x:20;y:20;width:400;height:32;value:0.95}
    Slider {id:vertical;x:20;y:80;width:32;height:240;orientation:Qt.Vertical;value:0.25}
    Slider {id:mirrored;x:100;y:100;width:400;height:46;value:0.7;LayoutMirroring.enabled:true}
    readonly property bool passed: [horizontal, vertical, mirrored].every(control => {
        const measurement = measure(control)
        return Math.abs(measurement.delta) < 0.01
            && measurement.handleWidth >= 20 && measurement.handleHeight >= 20
    }) && menu.U.Style.properties.border.left.size >= 1
        && popup.U.Style.properties.border.left.size >= 1
        && menu.U.Style.properties.background.color.a === 1
        && popup.U.Style.properties.background.color.a === 1
        && !Qt.colorEqual(menu.U.Style.properties.border.left.color, menu.U.Style.properties.background.color)
        && !Qt.colorEqual(popup.U.Style.properties.background.color, window.U.Style.properties.background.color)
    Menu {id:menu; MenuItem {text:"Criar novo"} MenuItem {text:"Propriedades"}}
    Popup {id:popup;width:200;height:120}
}
