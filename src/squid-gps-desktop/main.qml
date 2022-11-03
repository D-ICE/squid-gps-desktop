import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import backend 1.0

ApplicationWindow {
  title: qsTr("Squid GPS")
  visible: true
  width: 640
  height: 620
  color: "#192633"

  BackEnd {
    id: backend
  }

  // Current state
  Rectangle {
    id: position
    x: parent.width * 0.15
    y: 20
    width: parent.width * 0.7
    height: 100
    radius: 5
    color: "#2E3A47"

    Label {
      text: backend.current_state
      font.family: "Monospace"
      font.pixelSize: 13
      anchors.verticalCenter: parent.verticalCenter
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }

  // NMEA Frame decoder
  Rectangle {
    id: nmea_connection
    x: parent.width * 0.15
    y: position.y + position.height + 20
    width: parent.width * 0.7
    height: 250
    radius: 5
    color: "#2E3A47"

    Rectangle {
      id: nmea_decoder_title
      height: 40
      width: parent.width
      radius: parent.radius
      color: "#434E5A"

      Label {
        text: qsTr("NMEA 0183 UDP Listener")
        font.pixelSize: 15
        anchors.verticalCenter: parent.verticalCenter
        x: 10
      }
    }

    Rectangle {
      y: nmea_decoder_title.height - nmea_decoder_title.radius
      width: parent.width
      height: parent.height - nmea_decoder_title.height
      color: "#2E3A47"

      // Parameters
      Rectangle {
        id: nmea_decoder_parameters
        width: parent.width
        height: 60
        color: parent.color

        Label {
          id: port_label
          text: qsTr("Port")
          font.pixelSize: 15
          font.bold: true
          anchors.verticalCenter: parent.verticalCenter
          x: 20
        }

        TextField {
          height: 40
          width: 100
          anchors.verticalCenter: parent.verticalCenter
          x: parent.width - port_label.x - width
          text: backend.nmea_udp_port
          onEditingFinished: backend.nmea_udp_port = text
          validator: IntValidator {bottom: 1000; top: 65535}
        }
      }

      // Last received frames
      Rectangle {
        x: 20
        y: nmea_decoder_parameters.height + 10
        width: parent.width - 2 * x
        height: parent.height - y - 10
        color: "black"

        ScrollView {
          anchors.fill: parent
          TextArea {
            text: backend.nmea_displayed_frames
            color: "white"
          }
        }
      }
    }
  }

  // SquidX Connection
  Rectangle {
    id: squidx_connection
    x: parent.width * 0.15
    y: nmea_connection.height + nmea_connection.y + 20
    width: parent.width * 0.7
    height: 125
    radius: 5
    color: "#2E3A47"

    Rectangle {
      id: squidx_connection_title
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
      y: squidx_connection_title.height - squidx_connection_title.radius
      width: parent.width
      height: parent.height - squidx_connection_title.height
      color: "#2E3A47"

      Label {
        id: connection_label
        text: backend.squid_connection_status
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
        onClicked: backend.UpdateSquidState(checked)
        text: checked ? qsTr("Cancel") : qsTr("Connect")
        anchors.verticalCenter: parent.verticalCenter
        background: Rectangle {
          radius: 3
          color: parent.checked ? "#D75D5F" : "#4AF28E"
        }
      }
    }
  }

  // Roadbook Connection

  Rectangle {
    x: parent.width * 0.15
    y: squidx_connection.height + squidx_connection.y + 20
    width: parent.width * 0.7
    height: 40
    radius: 5
    color: "#434E5A"

    Label {
      id: roadbook_connection_label
      text: qsTr("Roadbook Connection")
      font.pixelSize: 15
      anchors.verticalCenter: parent.verticalCenter
      x: 10
    }

    CheckBox {
      x: parent.width - roadbook_connection_label.x - width
      checked: backend.connect_roadbook
      anchors.verticalCenter: parent.verticalCenter
      onCheckedChanged: backend.connect_roadbook = checked
    }
  }
}
