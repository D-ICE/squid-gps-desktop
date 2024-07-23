import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import Backend

Popup {
    id: rootItem

    parent: Overlay.overlay

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    width: Math.min(parent.width * 0.8, 600)

    clip: true
    closePolicy: Popup.CloseOnPressOutside
    modal: true
    padding: 20

    QtObject {
        id: _internal
        property SerialSettings settings: SerialSettings {}
    }

    onAboutToShow: { _internal.settings.from(BackEnd.serial_settings); }

    component SerialProperty: RowLayout {
        id: serialPropertyComponent
        property string name: ""
        property string prop: ""
        property bool useValueAsText: false
        Label {
            id: label
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            elide: Label.ElideRight
            wrapMode: Label.Wrap
            maximumLineCount: 2
            text: serialPropertyComponent.name + ":"
        }
        ComboBox {
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            model: _internal.settings[serialPropertyComponent.prop + "_choices"]
            textRole: serialPropertyComponent.useValueAsText ? "value" : "label"
            valueRole: "value"
            currentIndex: { count; return indexOfValue(_internal.settings[serialPropertyComponent.prop]); }
            onActivated: { _internal.settings[serialPropertyComponent.prop] = currentValue; }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 40
        ColumnLayout {
            spacing: 12
            SerialProperty {
                Layout.fillWidth: true
                name: qsTr("Baudrate")
                prop: "baudrate"
                useValueAsText: true
            }
            SerialProperty {
                Layout.fillWidth: true
                name: qsTr("Data bits")
                prop: "data_bits"
                useValueAsText: true
            }
            SerialProperty {
                Layout.fillWidth: true
                name: qsTr("Stop bits")
                prop: "stop_bits"
            }
            SerialProperty {
                Layout.fillWidth: true
                name: qsTr("Parity")
                prop: "parity"
            }
            SerialProperty {
                Layout.fillWidth: true
                name: qsTr("Flow control")
                prop: "flow_control"
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.maximumWidth: 120
            highlighted: true
            text: qsTr("Apply")
            enabled: !BackEnd.serial_settings.self.isSame(_internal.settings.self) //use 'self' to be notified of changes!
            onClicked: {
                BackEnd.serial_settings.from(_internal.settings);
                rootItem.close();
            }
        }
    }
}
