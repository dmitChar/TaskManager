import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


ApplicationWindow
{
    width: 1200
    height: 700
    visible: true
    title: "Task Manager"
    property string pageName: "Board"

    Drawer
    {
        id: drawer
        width: 260
        height: parent.height
        edge: Qt.LeftEdge
        background: Rectangle { color: "#2c3e50"}


        ColumnLayout
        {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            RowLayout
            {
                implicitHeight: 40
                Layout.fillWidth: true
                Layout.bottomMargin: 16

                Text
                {
                    text: "Task Manager"
                    font.pixelSize: 20
                    font.bold: true
                    color: "white"
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }
                Button
                {

                    icon.source: "qrc:/icons/icons/menu-burger.png"
                    font.pixelSize: 20
                    background: Rectangle {color: "transparent"}
                    Layout.alignment: Qt.AlignVCenter
                    onClicked:
                    {
                        drawer.close()
                    }
                }
            }

            ListView
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
            model:
                [
                    {iconQrc: "qrc:/icons/icons/edit.png", name: "Board"},
                    {iconQrc: "qrc:/icons/icons/chart-histogram.png", name: "Statistics"}
                ]

                delegate: MenuButton
                {
                    iconSource: modelData.iconQrc
                    name: modelData.name
                    onClicked:
                    {
                        pageName = modelData.name
                        drawer.close()
                        stackView.replace(pageName + ".qml")
                    }
                }
            }
        }
    }

    //topBar
    Rectangle
    {
        id: topBar
        height: 65
        width: parent.width
        color: "#2c3e50"
        z: 2

        RowLayout
        {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Button
            {
                icon.source: "qrc:/icons/icons/menu-burger.png"
                font.pixelSize: 20
                background: Rectangle {color: "transparent"}
                onClicked:
                {
                    drawer.open()
                }
            }

            TopBar
            {
                visible: pageName === "Board"
            }
        }
    }


    StackView
    {
        id:stackView
        anchors
        {
            top: topBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        initialItem: "Board.qml"
    }
}


