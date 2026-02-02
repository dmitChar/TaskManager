import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog

    property int taskIdToDelete: -1
    property string titleToDelete: ""

    signal deleteTask(int id)

    modal: true
    focus: true
    width: 420
    height: 250
    padding: 0

    parent: Overlay.overlay

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    background: Rectangle
    {
        radius: 14
        color: "white"
        border.color: "#dfe4ea"
        border.width: 1
    }

    header: Rectangle
    {
        height: 64
        radius: 14
        color: "#fdecea"
        implicitHeight: 40

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Rectangle
            {
                height: 32
                width: 32
                radius: 16
                Text
                {
                    text: "!"
                    font.pixelSize: 22
                    color: "#c0392b"
                    anchors.centerIn: parent
                }
            }

            Text {
                text: "Удаление задачи"
                font.pixelSize: 18
                font.bold: true
                color: "#c0392b"
                anchors.centerIn: parent

            }
        }
    }

    //Содержимое
    contentItem: Item {
            implicitHeight: 100

            Label {
                anchors.centerIn: parent
                anchors.margins: 24
                width: parent.width - 48

                text: qsTr("Вы уверены, что хотите удалить задачу\n«%1» ?").arg(titleToDelete)
                font.pixelSize: 16
                color: "#37474F"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                lineHeight: 1.4
            }
        }

    // ===== Footer / Buttons =====
    footer: ColumnLayout
    {
        RowLayout
        {
            spacing: 12
            Layout.fillWidth: true
            Layout.bottomMargin: 8

            Item { Layout.fillWidth: true }

            Button
            {
                text: "Отмена"
                implicitWidth: 140
                implicitHeight: 40

                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    font.bold: true
                    color: "#7f8c8d"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle
                {
                    radius: 8
                    color: parent.hovered ? "#ecf0f1" : "transparent"
                    border.color: "#dfe4ea"
                    border.width: 2

                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }
                onClicked: dialog.close()
            }

            Button {
                text: "Удалить"
                implicitWidth: 140
                implicitHeight: 40

                contentItem: Text
                {
                    text:parent.text
                    font.pixelSize: 14
                    font.bold: true
                    color: parent.enabled ? "white" : "#95a5a6"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle
                {
                    radius: 8
                    color:
                    {
                        if (!parent.enabled) return "#bdc3c7"
                        if (parent.pressed) return "#aa0000"
                        if (parent.hovered) return "#DC143C"
                        return "#aa0000"
                    }
                    Behavior on color
                    {
                        ColorAnimation
                        {
                            duration: 100
                        }
                    }
                }
                onClicked: dialog.accept()
            }
            Item { Layout.fillWidth: true }
        }
        Item { Layout.fillWidth: true }
    }

    onAccepted:
    {
        console.log("Удаляем задачу:", taskIdToDelete, titleToDelete)
        dialog.deleteTask(taskIdToDelete)
    }

}
