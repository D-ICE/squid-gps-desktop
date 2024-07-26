import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQml

import Backend

ApplicationWindow {
    id: appWindow
    title: qsTr("Squid GPS")
    visible: true
    minimumWidth: 600
    minimumHeight: 750

    Material.theme: Material.Dark
    Material.accent: Material.Teal
    Material.background: "#192633"

    readonly property InputsManager inputsManager: BackEnd.inputs_manager
    readonly property SerialReader serialReader: inputsManager.serial_reader
    readonly property UdpListener udpListener: inputsManager.udp_listener

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

    component ErrorLabel: Pane {
        id: errorLabelComponent
        padding: 8
        Material.background: Material.Red
        visible: text !== ""
        property alias text: label.text
        signal clear()
        contentItem: RowLayout {
            Label {
                id: label
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                padding: 4
                wrapMode: Label.Wrap
            }
            RoundButton {
                Layout.alignment: Qt.AlignVCenter
                icon.source: "fonts/trash.svg"
                flat: true
                onClicked: { errorLabelComponent.clear(); }
            }
        }
    }

    Flickable {
        anchors.fill: parent
        anchors.margins: 20

        contentHeight: columnLayout.height

        ColumnLayout {
            id: columnLayout

            width: parent.width * 0.7
            anchors.horizontalCenter: parent.horizontalCenter

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
                title: qsTr("NMEA Listener")
                Layout.fillWidth: true

                contentItem: Page {
                    padding: 8
                    header: TabBar {
                        id: bar
                        TabButton { text: qsTr("UDP") }
                        TabButton { text: qsTr("Serial") }
                    }
                    ColumnLayout {
                        width: parent.width
                        spacing: 12
                        StackLayout {
                            Layout.fillWidth: true
                            currentIndex: bar.currentIndex
                            ColumnLayout { //UDP item
                                Layout.fillWidth: true
                                readonly property bool isCurrentItem: StackLayout.isCurrentItem
                                onIsCurrentItemChanged: { udpErrorLabel.clear(); }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Label {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.fillWidth: true
                                        text: qsTr("UDP port") + " :"
                                        elide: Label.ElideRight
                                        wrapMode: Label.Wrap
                                        maximumLineCount: 2
                                    }
                                    TextField {
                                        id: udpPortTextField
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.fillWidth: true
                                        onEditingFinished: { appWindow.udpListener.port = text; }
                                        validator: IntValidator { bottom: 1000; top: 65535 }
                                        Material.accent: acceptableInput ? undefined : Material.Red
                                        Binding on text {
                                            value: appWindow.udpListener.port
                                            when: !udpPortTextField.activeFocus
                                            restoreMode: Binding.RestoreNone
                                        }
                                    }
                                }
                                Button {
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 120
                                    highlighted: true
                                    text: qsTr("Listen")
                                    enabled: udpPortTextField.acceptableInput
                                    onClicked: { appWindow.inputsManager.listenUDP(); }
                                }
                                ErrorLabel {
                                    id: udpErrorLabel
                                    Layout.fillWidth: true
                                    text: appWindow.udpListener.error_message
                                    onClear: { appWindow.udpListener.error_message = ""; }
                                }
                            }
                            ColumnLayout { //Serial item
                                Layout.fillWidth: true
                                readonly property bool isCurrentItem: StackLayout.isCurrentItem
                                onIsCurrentItemChanged: { serialErrorLabel.clear(); }
                                RowLayout {
                                    Layout.fillWidth: true
                                    RoundButton {
                                        Layout.alignment: Qt.AlignVCenter
                                        icon.source: "fonts/settings.svg"
                                        onClicked: { serialSettingsPopup.open(); }
                                    }
                                    Label {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.fillWidth: true
                                        text: qsTr("Serial port") + " :"
                                        elide: Label.ElideRight
                                        wrapMode: Label.Wrap
                                        maximumLineCount: 2
                                    }
                                    ComboBox {
                                        id: portnamesComboBox
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.fillWidth: true
                                        model: appWindow.serialReader.portnames_list
                                        currentIndex: -1 //prevent from automatically selecting the 1st element when list is created
                                        onCurrentValueChanged: { appWindow.serialReader.setSelectedPortname(currentValue); }
                                    }
                                    RoundButton {
                                        Layout.alignment: Qt.AlignVCenter
                                        icon.source: "fonts/refresh.svg"
                                        onClicked: { appWindow.serialReader.refreshPortnamesList(); }
                                    }
                                }
                                Button {
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 120
                                    highlighted: true
                                    text: qsTr("Read")
                                    enabled: portnamesComboBox.currentIndex !== -1
                                    onClicked: { appWindow.inputsManager.readSerial(); }
                                }
                                ErrorLabel {
                                    id: serialErrorLabel
                                    Layout.fillWidth: true
                                    text: appWindow.serialReader.error_message
                                    onClear: { appWindow.serialReader.error_message = ""; }
                                }
                            }
                        }
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 130
                            clip: true
                            background: Rectangle { color: "black" }
                            Label {
                                text: BackEnd.nmea_displayed_frames
                                wrapMode: Label.Wrap
                           }
                           wheelEnabled: false
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
                            validator: IntValidator { bottom: 1000; top: 65535 }
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
                            onClicked: { BackEnd.updateSquidState(checked, squidXPortTextField.text); }
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
                    onToggled: { BackEnd.connect_roadbook = checked; }
                }
            }
        }
    }
}
