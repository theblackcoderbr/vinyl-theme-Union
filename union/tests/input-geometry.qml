// SPDX-License-Identifier: GPL-2.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
ApplicationWindow {
 visible:true;width:640;height:400
 function spin(c){return {upX:c.up.indicator.x,downX:c.down.indicator.x,upY:c.up.indicator.y,downY:c.down.indicator.y,upW:c.up.indicator.width,downW:c.down.indicator.width,height:c.height}}
 function stacked(c) {
  const up=c.up.indicator, down=c.down.indicator
  return Math.abs(up.x-down.x)<0.01 && down.y>=up.y+up.height-0.01
      && up.x>=0 && up.x+up.width<=c.width+0.01
      && down.y+down.height<=c.height+0.01
 }
 readonly property bool passed: combo.contentItem.x-combo.leftInset>=7
     && combo.indicator.x+combo.indicator.width<=combo.width-combo.rightInset-7
     && stacked(s) && stacked(t) && stacked(mirrored)
 readonly property string report:JSON.stringify({combo:{textX:combo.contentItem.x,padding:combo.leftPadding,inset:combo.leftInset},spin:spin(s),constrained:spin(t),mirrored:spin(mirrored)})
 Column {x:40;y:40
 ComboBox{id:combo;width:420;model:["Generic | Generic 104-key PC","Outro"]}
 SpinBox{id:s;width:180;from:0;to:999;value:600}
 SpinBox{id:t;Layout.maximumWidth:100;width:100;from:0;to:999}
 SpinBox{id:mirrored;width:180;from:0;to:999;LayoutMirroring.enabled:true}
 }
}
