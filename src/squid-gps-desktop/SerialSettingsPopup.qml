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

    readonly property SerialSettings settings: BackEnd.inputs_manager.serial_reader.settings

    QtObject {
        id: _internal
        property SerialSettings tmp_settings: SerialSettings {}
    }

    onAboutToShow: { _internal.tmp_settings.from(rootItem.settings); }

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
            model: _internal.tmp_settings[serialPropertyComponent.prop + "_choices"]
            textRole: serialPropertyComponent.useValueAsText ? "value" : "label"
            valueRole: "value"
            currentIndex: { count; return indexOfValue(_internal.tmp_settings[serialPropertyComponent.prop]); }
            onActivated: { _internal.tmp_settings[serialPropertyComponent.prop] = currentValue; }
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
            enabled: !rootItem.settings.self.isSame(_internal.tmp_settings.self) //use 'self' to be notified of changes!
            onClicked: {
                rootItem.settings.from(_internal.tmp_settings);
                rootItem.close();
            }
        }
    }
}
