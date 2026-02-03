import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Button
{
    property string iconSource
    property string name

    Layout.fillWidth: true
    implicitHeight: 44

    background: Rectangle {
        radius: 8
        color: hovered ? "#34495e" : "transparent"
    }

    contentItem: Row {
        spacing: 12

        Image {
            source: iconSource
            width: 24
            height: 24
        }

        Text {
            text: name
            font.pixelSize: 15
            color: "white"
        }
    }
}
