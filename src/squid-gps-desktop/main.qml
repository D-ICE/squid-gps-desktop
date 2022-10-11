import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import backend 1.0

ApplicationWindow {
  title: qsTr("Squid GPS")
  visible: true
  width: 640
  height: 480
  color: "#192633"


  BackEnd {
    id: backend
  }

  Rectangle {
    x: parent.width * 0.15
    y: 20
    width: parent.width * 0.7
    height: 125
    radius: 5
    color: "#2E3A47"

    Rectangle {
      id: title
      height: 40
      width: parent.width
      radius: parent.radius
      color: "#434E5A"

      Label {
        text: qsTr("SquidX Connection")
        font.pixelSize: 15
        anchors.verticalCenter: parent.verticalCenter
        x: 10
      }
    }
    Rectangle {
      y: title.height - title.radius
      width: parent.width
      height: parent.height - title.height
      color: "#2E3A47"

      Label {
        id: connection_label
        text: backend.status
        font.pixelSize: 15
        font.bold: true
        anchors.verticalCenter: parent.verticalCenter
        x: 20
      }

      Button {
        height: 40
        width: 100
        x: parent.width - connection_label.x - width
        checkable: true
        onClicked: backend.UpdateState(checked)
        text: checked ? qsTr("Cancel") : qsTr("Connect")
        anchors.verticalCenter: parent.verticalCenter
        background: Rectangle {
          radius: 3
          color: parent.checked ? "#D75D5F" : "#4AF28E"
        }
      }
    }
  }
}
