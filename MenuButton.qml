import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Button
{
    property string iconSource
    property string name

    implicitHeight: 46
    width: parent.width

    background: Rectangle
    {
        radius: 8
        color: hovered ? "#34495e" : "transparent"
    }

    contentItem: Row
    {
        anchors.centerIn: parent
        spacing: 12
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        Image
        {
            source: iconSource
            width: 24
            height: 24
        }

        Text
        {
            text: name
            font.pixelSize: 15
            color: "white"
        }
    }
}
