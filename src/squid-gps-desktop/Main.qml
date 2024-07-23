import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import Backend

ApplicationWindow {
    id: appWindow
    title: qsTr("Squid GPS")
    visible: true
    width: 640
    height: columnLayout.implicitHeight + 80

    Material.theme: Material.Dark
    Material.accent: Material.Teal
    Material.background: "#192633"

    SerialSettingsPopup {
        id: serialSettingsPopup
    }

    component SquidGroupBox: GroupBox {
        id: control
        leftPadding: 20
        rightPadding: 20
        topPadding: label.height + 10
        bottomPadding: 10

        background: Rectangle {
            radius: 5
            color: "#2E3A47"
        }
        label: Label {
            height: text ? implicitHeight : 0
            text: control.title
            padding: 10
            width: parent.width
            font.pixelSize: 15
            background: Rectangle {
                radius: 5
                bottomLeftRadius: 0
                bottomRightRadius: 0
                color: "#434E5A"
            }
        }
    }

    ColumnLayout {
        id: columnLayout

        width: parent.width * 0.7
        anchors.centerIn: parent

        spacing: 20

        SquidGroupBox { // Current state
            Layout.fillWidth: true
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: BackEnd.current_state
                font.family: "Monospace"
                font.pixelSize: 13
            }
        }

        SquidGroupBox {
            title: qsTr("NMEA UDP and USB Listener")
            Layout.fillWidth: true

            Pane {
                id: pane
                property var highlightedColor: Material.accent
                property var backgroundColor: Material.background
                property color borderColor: "white"

                property double radius: 4
                property int inset: 4

                background: Rectangle {
                    color: pane.backgroundColor
                    radius: pane.radius
                }
                padding: 0
                contentItem: RowLayout {
                    id: row
                    property bool isActive: BackEnd.nmea_udp_active
                    Button {
                        text: qsTr("UDP")
                        topInset: pane.inset
                        bottomInset: pane.inset
                        rightInset: pane.inset
                        leftInset: pane.inset
                        Material.foreground: !row.isActive ? pane.borderColor : "black"
                        background: Rectangle {
                            color: row.isActive ? pane.highlightedColor : "transparent"
                            radius: pane.radius
                        }
                        onClicked: BackEnd.nmea_udp_active = true;
                    }
                    Button {
                        text: qsTr("USB")
                        topInset: pane.inset
                        bottomInset: pane.inset
                        rightInset: pane.inset
                        leftInset: pane.inset
                        Material.foreground: row.isActive ? pane.borderColor : "black"
                        background: Rectangle {
                            color: !row.isActive ? pane.highlightedColor : "transparent"
                            radius: pane.radius
                        }
                        onClicked: BackEnd.nmea_udp_active = false;
                    }
                }
            }
        }

        // USB (WIP)
        SquidGroupBox {
            title: qsTr("USB PORT")
            Layout.fillWidth: true
            RowLayout {
                RoundButton {
                    icon.source: "fonts/settings.svg"
                    onClicked: { serialSettingsPopup.open(); }
                }
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Serial Port")
                    font.pixelSize: 15
                    font.bold: true
                }

                ComboBox {
                    model: USB.availablePorts
                    onActivated: {
                        print("Hello: ", currentText)
                        //TODO : transaction!
                        BackEnd.transation(currentValue);
                        print("Hello: ", Qt.resolvedUrl("fonts/refresh.svg"))
                    }
                }

                RoundButton {
                    icon.source: "fonts/refresh.svg"
                    onClicked: {
                        USB.refresh();
                    }
                }
            }
        }

        // NMEA Frame decoder
        SquidGroupBox {
            title: qsTr("NMEA 0183 UDP Listener")
            Layout.fillWidth: true

            // Parameters
            contentItem: ColumnLayout {
                spacing: 20
                RowLayout {
                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Port")
                        font.pixelSize: 15
                        font.bold: true
                    }
                    TextField {
                        Layout.preferredWidth: 100
                        text: BackEnd.nmea_udp_port
                        onEditingFinished: BackEnd.nmea_udp_port = text
                        validator: IntValidator {bottom: 1000; top: 65535}
                    }
                }

                // Last received frames

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 130
                    background: Rectangle { color: "black" }
                    TextArea {
                        text: BackEnd.nmea_displayed_frames
                        color: "white"
                        readOnly: true
                    }
                }
            }
        }

        SquidGroupBox {
            Layout.fillWidth: true
            title: qsTr("SquidX Connection")

            contentItem: ColumnLayout {
                RowLayout {
                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Port")
                        font.pixelSize: 15
                        font.bold: true
                    }
                    TextField {
                        id: squidXPortTextField
                        text: "8000"
                        Layout.preferredWidth: 100
                        validator: IntValidator {bottom: 1000; top: 65535}
                    }
                }
                RowLayout {
                    Label {
                        text: BackEnd.squid_connection_status
                        font.pixelSize: 15
                        font.bold: true
                        Layout.fillWidth: true
                    }
                    Button {
                        checkable: true
                        onClicked: BackEnd.UpdateSquidState(checked, squidXPortTextField.text)
                        text: checked ? qsTr("Cancel") : qsTr("Connect")
                        background: Rectangle {
                            radius: 3
                            color: parent.checked ? "#D75D5F" : "#4AF28E"
                        }
                    }
                }
            }
        }

        SquidGroupBox {
            id: roadbookConnectionGroupBox
            Layout.fillWidth: true
            Layout.preferredHeight: label.implicitHeight
            title: qsTr("Roadbook Connection")
            CheckBox {
                parent: roadbookConnectionGroupBox.label
                anchors {
                    right: parent.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                checked: BackEnd.connect_roadbook
                onToggled: BackEnd.connect_roadbook = checked
            }
        }
    }
}
